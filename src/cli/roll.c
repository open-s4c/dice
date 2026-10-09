#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sxp.h>
#include <tmplr.h>
#include <unistd.h>

#include "slots.h"
#include "standard.h"
#include <sys/stat.h>

#define LIMIT    4096
#define COUNT(A) (sizeof(A) / sizeof((A)[0]))

struct identifier {
    char name[NAME_SIZE];
    int id;
    bool chain;
    bool standard;
};

struct subscription {
    char chain_name[NAME_SIZE], event_name[NAME_SIZE], handler[NAME_SIZE];
    char slot_name[NAME_SIZE];
    int chain, event, slot;
    bool callbacks;
};

static struct identifier ids[LIMIT];
static struct subscription subs[LIMIT];
static struct subscription prods[LIMIT];
static struct slot slots[SLOT_LIMIT];
static struct dependency {
    char slot[NAME_SIZE], reference[NAME_SIZE], scope[NAME_SIZE];
    bool before, absolute, slot_absolute, wildcard;
} deps[LIMIT];
static struct requirement {
    char reference[NAME_SIZE], scope[NAME_SIZE];
    unsigned line;
    bool absolute;
} requirements[LIMIT];
static size_t nrequirements;
static struct module_scope {
    char name[NAME_SIZE];
    unsigned fields;
    bool declared;
} namespaces[LIMIT];
static size_t nnamespaces;
static bool edges[SLOT_LIMIT][SLOT_LIMIT];
static int edge_specificity[SLOT_LIMIT][SLOT_LIMIT];
static size_t nids, nsubs, nprods, nslots, ndeps;
static int last_known_slot;
static struct callback_policy {
    char scope[NAME_SIZE];
    bool enabled;
} callback_policies[LIMIT];
static size_t ncallback_policies;
static bool callbacks, extension_callbacks;
static bool mempool_seen;
static uint64_t mempool_size = 200 * 1024 * 1024;
static char config_id[32];
static const char *input_name;
static char *input_paths[LIMIT];
static size_t ninputs;
static struct loaded_file {
    const char *path;
    char scope[NAME_SIZE];
    bool module_scope;
} loaded_files[LIMIT];
static size_t nloaded;
static struct import_path {
    char prefix[NAME_SIZE];
    const char *directory;
} import_paths[LIMIT];
static size_t nimport_paths;
static struct source_file {
    const char *name, *path;
    char *data;
} sources[] = {
    {"license", DICE_SOURCE_DIR "/LICENSE", NULL},
    {"call", DICE_SOURCE_DIR "/src/cli/templates/call.in", NULL},
    {"module", DICE_SOURCE_DIR "/src/cli/templates/module.in", NULL},
    {"public", DICE_SOURCE_DIR "/src/cli/templates/public.in", NULL},
};

static void __attribute__((format(printf, 1, 2), noreturn))
fail(const char *fmt, ...)
{
    fprintf(stderr, "dice-roll: %s: ", input_name ? input_name : "generator");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

static const char *
source_text(const char *name)
{
    for (size_t i = 0; i < COUNT(sources); i++) {
        struct source_file *source = &sources[i];
        if (strcmp(source->name, name))
            continue;
        if (source->data)
            return source->data;
        FILE *file = fopen(source->path, "rb");
        if (!file)
            fail("%s: %s", source->path, strerror(errno));
        if (fseek(file, 0, SEEK_END))
            fail("cannot seek %s", source->path);
        long length = ftell(file);
        if (length < 0 || (uintmax_t)length >= SIZE_MAX)
            fail("invalid file size: %s", source->path);
        rewind(file);
        source->data = malloc((size_t)length + 1);
        if (!source->data)
            fail("out of memory");
        if (fread(source->data, 1, (size_t)length, file) != (size_t)length ||
            ferror(file))
            fail("cannot read %s", source->path);
        source->data[length] = '\0';
        fclose(file);
        return source->data;
    }
    fail("unknown source file: %s", name);
}

static void
name_copy(char *dst, const char *src)
{
    if (!src || !*src || strlen(src) >= NAME_SIZE ||
        !(isalpha((unsigned char)*src) || *src == '_' || *src == '-'))
        fail("invalid configuration identifier: %s", src ? src : "(missing)");
    for (const char *s = src; *s; s++) {
        if (!(isalnum((unsigned char)*s) || *s == '_' || *s == '-'))
            fail("invalid configuration identifier: %s", src);
        *dst++ = *s == '-' ? '_' : *s;
    }
    *dst = 0;
}

static bool
name_equal(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if ((*a == '-' ? '_' : *a) != (*b == '-' ? '_' : *b))
            return false;
    return *a == *b;
}

static void
qualified_name(char *dst, const char *scope, const char *name)
{
    char component[NAME_SIZE];
    name_copy(component, name);
    if (!strcmp(component, "_"))
        fail("_ is reserved for scoped ordering defaults");
    if (snprintf(dst, NAME_SIZE, "%s%s%s", scope, *scope ? "/" : "",
                 component) >= NAME_SIZE)
        fail("qualified name too long: %s/%s", scope, name);
}

static void
macro_name(char *dst, const char *name)
{
    do {
        *dst++ = *name == '/' ? '_' : toupper((unsigned char)*name);
    } while (*name++);
}

static int
find_namespace(const char *name)
{
    for (size_t i = 0; i < nnamespaces; i++)
        if (!strcmp(namespaces[i].name, name))
            return (int)i;
    return -1;
}

static struct identifier *
find_id(const char *name, bool chain)
{
    for (size_t i = 0; i < nids; i++)
        if (ids[i].chain == chain && name_equal(ids[i].name, name))
            return &ids[i];
    return NULL;
}

static void
add_id(const char *name, int id, bool chain, bool standard)
{
    char normalized[NAME_SIZE];
    name_copy(normalized, name);
    name = normalized;
    if (nids == LIMIT)
        fail("too many identifiers (limit %d)", LIMIT);
    if (find_id(name, chain))
        fail("duplicate or reserved identifier: %s", name);
    if (!standard && strncmp(name, chain ? "CHAIN_" : "EVENT_", 6))
        fail("custom identifiers must start with %s",
             chain ? "CHAIN_" : "EVENT_");
    struct identifier *entry = &ids[nids++];
    name_copy(entry->name, name);
    entry->id       = id;
    entry->chain    = chain;
    entry->standard = standard;
}

static bool
symbol(const struct sxp *n, const char *name)
{
    return n && n->kind == SXP_SYMBOL && name_equal(n->text, name);
}

static const char *
atom(const struct sxp *n)
{
    if (!n || n->kind != SXP_SYMBOL)
        fail("line %u: expected a symbol", n ? n->line : 0);
    return n->text;
}

static const struct sxp *
form(const struct sxp *n)
{
    if (!n || n->kind != SXP_LIST || !n->child || n->child->kind != SXP_SYMBOL)
        fail("line %u: expected a named list", n ? n->line : 0);
    return n->child;
}

static const struct sxp *
single(const struct sxp *head)
{
    if (!head->next || head->next->next)
        fail("line %u: %s expects one argument", head->line, head->text);
    return head->next;
}

static int64_t
number(const struct sxp *n, int64_t min, int64_t max)
{
    const char *s = atom(n);
    char *end;
    errno           = 0;
    long long value = strtoll(s, &end, 10);
    if (errno || end == s || *end || value < min || value > max)
        fail("line %u: expected integer in %" PRId64 "..%" PRId64, n->line, min,
             max);
    return value;
}

static uint64_t
memory_size(const struct sxp *n)
{
    const char *s = atom(n);
    char *end;
    errno                  = 0;
    long long value        = strtoll(s, &end, 10);
    uint64_t scale         = *end ? 0 : 1;
    const char *suffixes[] = {"Ki", "Mi", "Gi", "Ti"};
    for (size_t i = 0; i < COUNT(suffixes); i++)
        if (!strcmp(end, suffixes[i]))
            scale = UINT64_C(1) << (10 * (i + 1));
    const uint64_t maximum = UINT64_C(1) << 40;
    if (errno || end == s || value < 0 || !scale ||
        (uint64_t)value > maximum / scale || (uint64_t)value * scale < 4096)
        fail(
            "line %u: expected memory size in 4Ki..1Ti "
            "(bytes or an integer with Ki, Mi, Gi, Ti suffix)",
            n->line);
    return (uint64_t)value * scale;
}

static void
read_ids(const struct sxp *args, bool chain)
{
    for (const struct sxp *n = args; n; n = n->next) {
        const char *name;
        int id = -1;
        if (n->kind == SXP_LIST) {
            const struct sxp *head = form(n);
            name                   = atom(head);
            id                     = (int)number(single(head), 1, 65534);
        } else {
            name = atom(n);
        }
        add_id(name, id, chain, false);
    }
}

static int
find_slot(const char *name)
{
    for (size_t i = 0; i < nslots; i++)
        if (name_equal(slots[i].name, name))
            return (int)i;
    return -1;
}

static int
add_slot(const char *name, bool implicit)
{
    if (find_slot(name) >= 0)
        fail("duplicate slot: %s", name);
    if (!implicit && find_namespace(name) >= 0)
        fail("name is both a slot and a module: %s", name);
    if (nslots == SLOT_LIMIT)
        fail("too many slots (limit %d)", SLOT_LIMIT);
    struct slot *s = &slots[nslots];
    strcpy(s->name, name);
    char macro[NAME_SIZE];
    macro_name(macro, name);
    if (!strcmp(macro, "DICE_CORE"))
        fail("reserved slot dice-core belongs to the core at slot 0");
    for (size_t i = 0; i < nslots; i++) {
        char other[NAME_SIZE];
        macro_name(other, slots[i].name);
        if (!strcmp(macro, other))
            fail("slot names collide in generated macros: %s and %s", name,
                 slots[i].name);
    }
    s->implicit = implicit;
    s->number = s->forced = -1;
    return (int)nslots++;
}

/* "default" is a virtual reference to a module's implicit slot. */
static void
declaration_name(const char *name)
{
    const char *last = strrchr(name, '/');
    if (!strcmp(last ? last + 1 : name, "default"))
        fail("default is reserved for a module's implicit slot");
}

static int
add_namespace(const char *name)
{
    if (!*name)
        return -1;
    int index = find_namespace(name);
    if (index >= 0)
        return index;
    declaration_name(name);
    int slot = find_slot(name);
    if (slot >= 0 && !slots[slot].implicit)
        fail("name is both a slot and a module: %s", name);
    char parent[NAME_SIZE];
    strcpy(parent, name);
    char *last = strrchr(parent, '/');
    if (last) {
        *last = 0;
        add_namespace(parent);
    }
    if (nnamespaces == LIMIT)
        fail("too many namespaces");
    strcpy(namespaces[nnamespaces].name, name);
    return (int)nnamespaces++;
}

static int
module_slot(const char *scope)
{
    int index = find_slot(scope);
    return index >= 0 ? index : add_slot(scope, true);
}

/* Ordering lists are absolute paths; symbols use lexical scope. */
static void
read_reference(char *name, const struct sxp *ref)
{
    if (ref->kind != SXP_LIST) {
        name_copy(name, atom(ref));
        if (!strcmp(name, "_"))
            fail("line %u: _ requires a standalone ordering default",
                 ref->line);
        return;
    }
    if (!ref->child)
        fail("line %u: empty ordering reference", ref->line);
    *name = 0;
    for (const struct sxp *part = ref->child; part; part = part->next) {
        char next[NAME_SIZE];
        qualified_name(next, name, atom(part));
        strcpy(name, next);
    }
}

static void
read_routes(const struct sxp *args, const char *slot, bool publishes)
{
    for (const struct sxp *n = args; n; n = n->next) {
        const struct sxp *head = form(n);
        if (!head->next)
            fail("line %u: chain %s requires at least one event", n->line,
                 head->text);
        for (const struct sxp *event = head->next; event; event = event->next) {
            size_t *count = publishes ? &nprods : &nsubs;
            if (*count == LIMIT)
                fail("too many routes");
            struct subscription *r = &(publishes ? prods : subs)[(*count)++];
            strcpy(r->slot_name, slot);
            name_copy(r->chain_name, atom(head));
            name_copy(r->event_name, atom(event));
        }
    }
}

static void
read_callbacks(const struct sxp *key, const char *scope)
{
    for (size_t i = 0; i < ncallback_policies; i++)
        if (!strcmp(callback_policies[i].scope, scope))
            fail("line %u: duplicate setting callbacks in %s", key->line,
                 *scope ? scope : "top level");
    const struct sxp *value = single(key);
    if (!symbol(value, "true") && !symbol(value, "false"))
        fail("line %u: callbacks expects true or false", value->line);
    if (ncallback_policies == LIMIT)
        fail("too many callback policies");
    struct callback_policy *policy = &callback_policies[ncallback_policies++];
    strcpy(policy->scope, scope);
    policy->enabled = symbol(value, "true");
}

/* Resolve after all imports and reopened modules have contributed settings. */
static bool
callbacks_allowed(const char *scope)
{
    char current[NAME_SIZE];
    strcpy(current, scope);
    for (;;) {
        for (size_t i = 0; i < ncallback_policies; i++)
            if (!strcmp(callback_policies[i].scope, current))
                return callback_policies[i].enabled;
        if (!*current)
            return true;
        char *last = strrchr(current, '/');
        if (last)
            *last = 0;
        else
            *current = 0;
    }
}

static void
read_require(const struct sxp *key, const char *scope)
{
    if (!key->next)
        fail("line %u: require expects at least one reference", key->line);
    for (const struct sxp *ref = key->next; ref; ref = ref->next) {
        if (nrequirements == LIMIT)
            fail("too many requirements");
        struct requirement *r = &requirements[nrequirements++];
        read_reference(r->reference, ref);
        strcpy(r->scope, scope);
        r->line     = ref->line;
        r->absolute = ref->kind == SXP_LIST;
    }
}

static void
read_slot_field(const struct sxp *key, const char *name, const char *scope,
                int index, unsigned *seen)
{
    unsigned bit;
    if (symbol(key, "number"))
        bit = 1;
    else if (symbol(key, "before"))
        bit = 2;
    else if (symbol(key, "after"))
        bit = 4;
    else if (symbol(key, "subscribes"))
        bit = 8;
    else if (symbol(key, "publishes"))
        bit = 16;
    else if (symbol(key, "callbacks"))
        bit = 32;
    else if (symbol(key, "require"))
        bit = 64;
    else
        fail("line %u: unknown slot field %s", key->line, key->text);
    if (*seen & bit)
        fail("line %u: duplicate slot field %s", key->line, key->text);
    *seen |= bit;
    if (bit == 1)
        slots[index].forced = (int)number(single(key), 1, INT_MAX);
    else if (bit == 8 || bit == 16)
        read_routes(key->next, name, bit == 16);
    else if (bit == 32)
        read_callbacks(key, name);
    else if (bit == 64)
        read_require(key, scope);
    else {
        if (!key->next)
            fail("line %u: empty ordering constraint", key->line);
        for (const struct sxp *ref = key->next; ref; ref = ref->next) {
            if (ndeps == LIMIT)
                fail("too many ordering constraints");
            struct dependency *dep = &deps[ndeps++];
            strcpy(dep->slot, name);
            strcpy(dep->scope, scope);
            read_reference(dep->reference, ref);
            dep->before        = bit == 2;
            dep->absolute      = ref->kind == SXP_LIST;
            dep->slot_absolute = true;
        }
    }
}

static void
read_slot(const struct sxp *head, const char *scope)
{
    char name[NAME_SIZE];
    qualified_name(name, scope, atom(head->next));
    declaration_name(name);
    int index     = add_slot(name, false);
    unsigned seen = 0;
    for (const struct sxp *n = head->next->next; n; n = n->next)
        read_slot_field(form(n), name, scope, index, &seen);
}

/* A one-reference rule in a module orders the entire module. */
static void
read_module_order(const struct sxp *key, const char *scope)
{
    if (ndeps == LIMIT)
        fail("too many ordering constraints");
    struct dependency *dep = &deps[ndeps++];
    strcpy(dep->slot, scope);
    strcpy(dep->scope, scope);
    read_reference(dep->reference, key->next);
    dep->before        = symbol(key, "before");
    dep->absolute      = key->next->kind == SXP_LIST;
    dep->slot_absolute = true;
}

static void
read_order(const struct sxp *key, const char *scope)
{
    const struct sxp *left = key->next;
    if (!left || !left->next || left->next->next)
        fail("line %u: %s expects two ordering references", key->line,
             key->text);
    if (ndeps == LIMIT)
        fail("too many ordering constraints");
    struct dependency *dep = &deps[ndeps++];
    strcpy(dep->scope, scope);
    dep->wildcard = symbol(left, "_");
    if (!dep->wildcard)
        read_reference(dep->slot, left);
    read_reference(dep->reference, left->next);
    dep->before        = symbol(key, "before");
    dep->slot_absolute = left->kind == SXP_LIST;
    dep->absolute      = left->next->kind == SXP_LIST;
}

static void
read_mempool(const struct sxp *key)
{
    if (mempool_seen)
        fail("line %u: duplicate setting %s", key->line, key->text);
    mempool_seen = true;
    mempool_size = memory_size(single(key));
}

static void read_config(const char *, const char *[], int, bool, const char *,
                        bool);

static const char *
record_input(const char *path)
{
    char resolved[PATH_MAX];
    if (!realpath(path, resolved))
        fail("%s: %s", path, strerror(errno));
    for (size_t i = 0; i < ninputs; i++)
        if (!strcmp(input_paths[i], resolved))
            return input_paths[i];
    if (ninputs == LIMIT)
        fail("too many configuration inputs");
    char *copy = strdup(resolved);
    if (!copy)
        fail("out of memory");
    input_paths[ninputs++] = copy;
    return copy;
}

static void
add_import_path(const char *argument)
{
    if (!*argument || nimport_paths == COUNT(import_paths))
        fail("invalid or too many import search paths");
    struct import_path *entry = &import_paths[nimport_paths++];
    const char *equal         = strchr(argument, '=');
    entry->directory          = equal ? equal + 1 : argument;
    if (!*entry->directory)
        fail("empty import directory: %s", argument);
    if (!equal)
        return;
    size_t length = (size_t)(equal - argument);
    if (!length || length >= sizeof(entry->prefix))
        fail("invalid namespace mapping: %s", argument);
    memcpy(entry->prefix, argument, length);
    entry->prefix[length] = 0;
    const char *part      = entry->prefix;
    while (part) {
        const char *end = strchr(part, '/');
        size_t size     = end ? (size_t)(end - part) : strlen(part);
        char component[NAME_SIZE], checked[NAME_SIZE];
        memcpy(component, part, size);
        component[size] = 0;
        name_copy(checked, component);
        part = end ? end + 1 : NULL;
    }
}

static bool
import_candidate(char *path, const struct import_path *root,
                 const char *relative, bool file)
{
    size_t prefix = strlen(root->prefix);
    if (prefix) {
        if (strncmp(relative, root->prefix, prefix) ||
            (relative[prefix] && relative[prefix] != '/'))
            return false;
        relative += prefix;
        if (*relative)
            relative++;
    }
    /* A mount point itself denotes a directory, never directory.dice. */
    if (file && !*relative)
        return false;
    if (snprintf(path, PATH_MAX, "%s%s%s%s", root->directory,
                 *relative ? "/" : "", relative,
                 file ? ".dice" : "") >= PATH_MAX)
        fail("import path too long");
    return true;
}

/* Watch import search locations for new overrides and bulk-import members.
 * For missing candidates, watch the nearest existing parent for creation. */
static void
watch_directory(const char *path)
{
    char parent[PATH_MAX];
    strcpy(parent, path);
    struct stat info;
    while (stat(parent, &info)) {
        if (errno != ENOENT && errno != ENOTDIR)
            fail("%s: %s", parent, strerror(errno));
        char *last = strrchr(parent, '/');
        if (!last) {
            strcpy(parent, ".");
            break;
        }
        if (last == parent) {
            parent[1] = 0;
            break;
        }
        *last = 0;
    }
    record_input(parent);
}

struct import_file {
    char *name, *path;
};

static int
import_compare(const void *a, const void *b)
{
    return strcmp(((const struct import_file *)a)->name,
                  ((const struct import_file *)b)->name);
}

static void
read_import(const struct sxp *library, const char *ancestors[], int depth)
{
    if (library->kind != SXP_LIST || !library->child)
        fail("line %u: import expects a nonempty library name", library->line);
    char relative[PATH_MAX] = "", scope[NAME_SIZE] = "", parent[NAME_SIZE];
    size_t length = 0;
    for (const struct sxp *n = library->child; n; n = n->next) {
        const char *part = atom(n);
        strcpy(parent, scope);
        qualified_name(scope, parent, part);
        /* Library components are literal file names, not C identifiers. */
        int added = snprintf(relative + length, sizeof(relative) - length,
                             "%s%s", length ? "/" : "", part);
        if (added < 0 || (size_t)added >= sizeof(relative) - length)
            fail("import path too long");
        length += (size_t)added;
    }
    /* A file import takes precedence over a directory import. */
    for (size_t i = 0; i < nimport_paths; i++) {
        char candidate[PATH_MAX];
        if (!import_candidate(candidate, &import_paths[i], relative, true))
            continue;
        struct stat info;
        if (stat(candidate, &info)) {
            if (errno == ENOENT || errno == ENOTDIR) {
                watch_directory(candidate);
                continue;
            }
            fail("%s: %s", candidate, strerror(errno));
        }
        if (!S_ISREG(info.st_mode))
            fail("import is not a regular file: %s", candidate);
        read_config(candidate, ancestors, depth + 1, true, parent, false);
        return;
    }

    struct import_file *files = calloc(LIMIT, sizeof(*files));
    if (!files)
        fail("out of memory");
    size_t count = 0;
    bool found   = false;
    for (size_t i = 0; i < nimport_paths; i++) {
        char directory[PATH_MAX];
        if (!import_candidate(directory, &import_paths[i], relative, false))
            continue;
        watch_directory(directory);
        DIR *dir = opendir(directory);
        if (!dir) {
            if (errno == ENOENT || errno == ENOTDIR)
                continue;
            fail("%s: %s", directory, strerror(errno));
        }
        found = true;
        for (;;) {
            errno                = 0;
            struct dirent *entry = readdir(dir);
            if (!entry) {
                if (errno)
                    fail("%s: %s", directory, strerror(errno));
                break;
            }
            size_t size = strlen(entry->d_name);
            if (size <= 5 || strcmp(entry->d_name + size - 5, ".dice"))
                continue;
            bool duplicate = false;
            for (size_t j = 0; j < count; j++)
                if (!strcmp(files[j].name, entry->d_name))
                    duplicate = true;
            if (duplicate)
                continue;
            char path[PATH_MAX];
            if (snprintf(path, sizeof(path), "%s/%s", directory,
                         entry->d_name) >= (int)sizeof(path))
                fail("import path too long");
            struct stat info;
            if (stat(path, &info))
                fail("%s: %s", path, strerror(errno));
            if (!S_ISREG(info.st_mode))
                continue;
            char stem[NAME_SIZE], checked[NAME_SIZE];
            if (size - 5 >= sizeof(stem))
                fail("import filename too long: %s", path);
            memcpy(stem, entry->d_name, size - 5);
            stem[size - 5] = 0;
            name_copy(checked, stem);
            if (count == LIMIT)
                fail("too many files in directory import");
            files[count].name = strdup(entry->d_name);
            files[count].path = strdup(path);
            if (!files[count].name || !files[count].path)
                fail("out of memory");
            count++;
        }
        closedir(dir);
    }
    if (!found)
        fail("line %u: cannot find import %s (use -I DIRECTORY)", library->line,
             relative);
    add_namespace(scope);
    qsort(files, count, sizeof(*files), import_compare);
    for (size_t i = 0; i < count; i++) {
        read_config(files[i].path, ancestors, depth + 1, true, scope, false);
        free(files[i].name);
        free(files[i].path);
    }
    free(files);
}

static void
read_forms(const struct sxp *forms, const char *path, const char *ancestors[],
           int depth, const char *scope, bool module_scope)
{
    bool schema = false;
    for (const struct sxp *n = forms; n; n = n->next) {
        const struct sxp *key = form(n);
        if (symbol(key, "schema")) {
            if (schema)
                fail("duplicate schema");
            (void)number(single(key), 1, 1);
            schema = true;
        } else if (symbol(key, "events") || symbol(key, "chains")) {
            read_ids(key->next, symbol(key, "chains"));
        } else if (symbol(key, "callbacks")) {
            read_callbacks(key, scope);
        } else if (symbol(key, "mempool_size")) {
            read_mempool(key);
        } else if (symbol(key, "slot")) {
            if (!module_scope)
                fail("line %u: slot declarations must be inside a module",
                     key->line);
            read_slot(key, scope);
        } else if (symbol(key, "number") || symbol(key, "subscribes") ||
                   symbol(key, "publishes")) {
            if (!module_scope)
                fail("line %u: %s requires a module or slot", key->line,
                     key->text);
            int index                   = module_slot(scope);
            struct module_scope *module = &namespaces[find_namespace(scope)];
            read_slot_field(key, scope, scope, index, &module->fields);
        } else if (symbol(key, "before") || symbol(key, "after")) {
            if (module_scope && key->next && !key->next->next)
                read_module_order(key, scope);
            else
                read_order(key, scope);
        } else if (symbol(key, "require")) {
            read_require(key, scope);
        } else if (symbol(key, "module")) {
            char nested[NAME_SIZE];
            qualified_name(nested, scope, atom(key->next));
            int index                  = add_namespace(nested);
            namespaces[index].declared = true;
            read_forms(key->next->next, path, ancestors, depth, nested, true);
        } else if (symbol(key, "import")) {
            if (!key->next)
                fail("import requires at least one library name");
            for (const struct sxp *p = key->next; p; p = p->next)
                read_import(p, ancestors, depth);
        } else if (symbol(key, "include")) {
            if (!key->next)
                fail("include requires at least one path");
            for (const struct sxp *p = key->next; p; p = p->next) {
                if (p->kind != SXP_STRING || !*p->text)
                    fail("line %u: include expects quoted paths", p->line);
                char next[PATH_MAX];
                int len;
                if (*p->text == '/')
                    len = snprintf(next, sizeof(next), "%s", p->text);
                else
                    len = snprintf(next, sizeof(next), "%.*s/%s",
                                   (int)(strrchr(path, '/') - path), path,
                                   p->text);
                if (len < 0 || (size_t)len >= sizeof(next))
                    fail("include path too long");
                read_config(next, ancestors, depth + 1, false, scope,
                            module_scope);
            }
        } else {
            fail("%s:%u: unknown form %s", path, key->line, key->text);
        }
    }
}

static void
read_config(const char *path, const char *ancestors[], int depth, bool once,
            const char *scope, bool module_scope)
{
    if (depth == 32)
        fail("configuration includes/imports exceed 32 levels");
    const char *resolved = record_input(path);
    for (int i = 0; i < depth; i++)
        if (!strcmp(ancestors[i], resolved))
            fail("include cycle or import cycle involving %s", resolved);
    if (once)
        for (size_t i = 0; i < nloaded; i++)
            if (!strcmp(loaded_files[i].path, resolved) &&
                !strcmp(loaded_files[i].scope, scope) &&
                loaded_files[i].module_scope == module_scope)
                return;
    if (nloaded == LIMIT)
        fail("too many configuration inputs");
    loaded_files[nloaded].module_scope = module_scope;
    loaded_files[nloaded].path         = resolved;
    strcpy(loaded_files[nloaded++].scope, scope);
    ancestors[depth] = resolved;
    add_namespace(scope);
    FILE *file = fopen(resolved, "r");
    if (!file)
        fail("%s: %s", resolved, strerror(errno));
    struct sxp_error error;
    struct sxp *root = sxp_read_all(file, &error);
    fclose(file);
    if (!root)
        fail("%s:%u: %s", resolved, error.line, error.message);
    const struct sxp *forms = root->child;
    read_forms(forms, resolved, ancestors, depth, scope, module_scope);
    sxp_free(root);
}

static int
id_compare(const void *a, const void *b)
{
    const struct identifier *x = a, *y = b;
    return x->chain != y->chain ? (int)x->chain - (int)y->chain :
                                  strcmp(x->name, y->name);
}

static int
sub_compare(const void *a, const void *b)
{
    const struct subscription *x = a, *y = b;
    if (x->chain != y->chain)
        return x->chain < y->chain ? -1 : 1;
    if (x->slot != y->slot)
        return x->slot < y->slot ? -1 : 1;
    return x->event - y->event;
}

static int
slot_compare(const void *a, const void *b)
{
    return strcmp(((const struct slot *)a)->name,
                  ((const struct slot *)b)->name);
}

static int
resolve_reference(const char *name, const char *scope, bool absolute,
                  char *resolved)
{
    char current[NAME_SIZE];
    strcpy(current, scope);
    for (;;) {
        if (absolute)
            strcpy(resolved, name);
        else
            qualified_name(resolved, current, name);
        size_t length = strlen(resolved);
        if (length > 8 && !strcmp(resolved + length - 8, "/default")) {
            resolved[length - 8] = 0;
            int slot             = find_slot(resolved);
            if (slot >= 0 && slots[slot].implicit)
                return slot;
        } else {
            if (find_namespace(resolved) >= 0)
                return -1; /* A module includes its default and nested slots. */
            int slot = find_slot(resolved);
            if (slot >= 0)
                return slot;
        }
        if (absolute || !*current)
            return -2; /* An optional ordering endpoint is absent. */
        char *last = strrchr(current, '/');
        if (last)
            *last = 0;
        else
            *current = 0;
    }
}

static bool
reference_matches(size_t slot, int target, const char *name)
{
    if (target >= 0)
        return (int)slot == target;
    size_t length = strlen(name);
    return (slots[slot].implicit && !strcmp(slots[slot].name, name)) ||
           (!strncmp(slots[slot].name, name, length) &&
            slots[slot].name[length] == '/');
}

static bool
slot_in_scope(size_t slot, const char *scope)
{
    return !*scope || reference_matches(slot, -1, scope);
}

static void
add_order(size_t before, size_t after, int specificity)
{
    if (specificity < edge_specificity[before][after])
        return;
    if (specificity > edge_specificity[before][after]) {
        /* Override only this pair, in either direction. Equal-specificity
         * contradictions retain both edges for the cycle diagnostic. */
        edges[before][after] = edges[after][before] = false;
        edge_specificity[before][after] = edge_specificity[after][before] =
            specificity;
    }
    edges[before][after] = true;
}

/* Empty leaf modules still name a slot. Containers need no unused default. */
static void
prepare_modules(void)
{
    for (size_t i = 0; i < nnamespaces; i++) {
        const struct module_scope *module = &namespaces[i];
        if (!module->declared || find_slot(module->name) >= 0)
            continue;
        bool children = false;
        size_t length = strlen(module->name);
        for (size_t j = 0; j < nslots; j++)
            if (!strncmp(slots[j].name, module->name, length) &&
                slots[j].name[length] == '/')
                children = true;
        for (size_t j = 0; j < nnamespaces; j++)
            if (namespaces[j].declared &&
                !strncmp(namespaces[j].name, module->name, length) &&
                namespaces[j].name[length] == '/')
                children = true;
        if (!children)
            module_slot(module->name);
    }
}

static void
prepare_slots(void)
{
    prepare_modules();
    qsort(slots, nslots, sizeof(*slots), slot_compare);
    for (size_t i = 0; i < nrequirements; i++) {
        const struct requirement *r = &requirements[i];
        char resolved[NAME_SIZE];
        if (resolve_reference(r->reference, r->scope, r->absolute, resolved) ==
            -2)
            fail(
                "line %u: required slot or namespace is absent: %s "
                "(scope %s)",
                r->line, r->reference, *r->scope ? r->scope : "top level");
    }
    for (size_t i = 0; i < ndeps; i++) {
        const struct dependency *dep = &deps[i];
        char left[NAME_SIZE], right[NAME_SIZE];
        int target =
            resolve_reference(dep->reference, dep->scope, dep->absolute, right);
        int owner = dep->wildcard ? -1 :
                                    resolve_reference(dep->slot, dep->scope,
                                                      dep->slot_absolute, left);
        if (target == -2 || owner == -2)
            continue;
        int specificity = INT_MAX; /* Named constraints override defaults. */
        if (dep->wildcard) {
            specificity = *dep->scope ? 2 : 1;
            for (const char *p = dep->scope; *p; p++)
                specificity += *p == '/';
        }
        for (size_t j = 0; j < nslots; j++)
            if (dep->wildcard ? slot_in_scope(j, dep->scope) &&
                                    !reference_matches(j, target, right) :
                                reference_matches(j, owner, left))
                for (size_t k = 0; k < nslots; k++) {
                    if (!reference_matches(k, target, right))
                        continue;
                    if (dep->before)
                        add_order(j, k, specificity);
                    else
                        add_order(k, j, specificity);
                }
    }
    char error[2048];
    if (slots_resolve(slots, nslots, edges, error, sizeof(error)))
        fail("%s", error);
    for (size_t i = 0; i < nslots; i++)
        if (slots[i].number > last_known_slot)
            last_known_slot = slots[i].number;
}

static void
resolve_routes(struct subscription *routes, size_t count, bool publishes)
{
    for (size_t i = 0; i < count; i++) {
        struct subscription *r   = &routes[i];
        struct identifier *chain = find_id(r->chain_name, true);
        struct identifier *event = find_id(r->event_name, false);
        int owner                = find_slot(r->slot_name);
        if (!chain || !event || owner < 0)
            fail("unknown chain/event in %s: %s/%s", r->slot_name,
                 r->chain_name, r->event_name);
        r->chain     = chain->id;
        r->event     = event->id;
        r->slot      = slots[owner].number;
        r->callbacks = callbacks_allowed(r->slot_name);
        if (!publishes)
            snprintf(r->handler, sizeof(r->handler), "dice_slot_%d_%d_%d",
                     r->chain, r->event, r->slot);
    }
    qsort(routes, count, sizeof(*routes), sub_compare);
    for (size_t i = 1; i < count; i++)
        if (!sub_compare(&routes[i - 1], &routes[i]))
            fail("duplicate %s for slot %d, chain %d, event %d",
                 publishes ? "publication" : "subscription", routes[i].slot,
                 routes[i].chain, routes[i].event);
}

static uint64_t
hash_string(uint64_t hash, const char *value)
{
    do {
        hash ^= (unsigned char)*value;
        hash *= UINT64_C(1099511628211);
    } while (*value++);
    return hash;
}

static void
resolve(void)
{
    /* Explicit declarations only pin IDs or introduce otherwise unused names.
     * Slot endpoints supply the rest of the event and chain universe. */
    for (size_t i = 0; i < nsubs + nprods; i++) {
        struct subscription *r = i < nsubs ? &subs[i] : &prods[i - nsubs];
        if (!find_id(r->chain_name, true))
            add_id(r->chain_name, -1, true, false);
        if (!find_id(r->event_name, false))
            add_id(r->event_name, -1, false, false);
    }
    qsort(ids, nids, sizeof(*ids), id_compare);
    for (size_t i = 0; i < nids; i++) {
        if (ids[i].id < 0) {
            int candidate = ids[i].chain ? 7 : 128;
            bool used;
            do {
                used = false;
                for (size_t j = 0; j < nids; j++)
                    if (ids[j].chain == ids[i].chain && ids[j].id == candidate)
                        used = true;
                if (used)
                    candidate++;
            } while (used);
            if (candidate > 65534)
                fail("ID space exhausted");
            ids[i].id = candidate;
        }
        if (!ids[i].standard)
            for (size_t j = 0; j < nids; j++)
                if (i != j && ids[j].chain == ids[i].chain &&
                    ids[j].id == ids[i].id)
                    fail("ID collision: %s and %s", ids[i].name, ids[j].name);
    }
    prepare_slots();
    resolve_routes(subs, nsubs, false);
    resolve_routes(prods, nprods, true);
    extension_callbacks = callbacks_allowed("");
    callbacks           = extension_callbacks;
    for (size_t i = 0; i < nsubs; i++)
        callbacks |= subs[i].callbacks;
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < COUNT(sources); i++)
        hash = hash_string(hash, source_text(sources[i].name));
    char entry[256];
    for (size_t i = 0; i < nids; i++) {
        snprintf(entry, sizeof(entry), "%d:%.*s:%d", ids[i].chain,
                 NAME_SIZE - 1, ids[i].name, ids[i].id);
        hash = hash_string(hash, entry);
    }
    for (size_t i = 0; i < nsubs; i++) {
        snprintf(entry, sizeof(entry), "%d:%d:%d:%.*s", subs[i].chain,
                 subs[i].event, subs[i].slot, NAME_SIZE - 1, subs[i].handler);
        hash = hash_string(hash, entry);
    }
    for (size_t i = 0; i < nslots; i++) {
        snprintf(entry, sizeof(entry), "slot:%.*s:%d:%d", NAME_SIZE - 1,
                 slots[i].name, slots[i].number,
                 callbacks_allowed(slots[i].name));
        hash = hash_string(hash, entry);
    }
    for (size_t i = 0; i < nprods; i++) {
        snprintf(entry, sizeof(entry), "publishes:%d:%d:%d", prods[i].chain,
                 prods[i].event, prods[i].slot);
        hash = hash_string(hash, entry);
    }
    snprintf(entry, sizeof(entry), "%d:%" PRIu64, extension_callbacks,
             mempool_size);
    hash = hash_string(hash, entry);
    snprintf(config_id, sizeof(config_id), "roll-1-%016" PRIx64, hash);
}

static int
sink(const char *buffer, size_t length, void *file)
{
    return fwrite(buffer, 1, length, file) != length;
}

static void
render(FILE *out, const char *source, const char *prefix, const char *key,
       const char *value)
{
    tmplr_opts opts = {.max_block_lines = 4096, .prefix = prefix};
    tmplr_ctx *ctx  = tmplr_create(&opts);
    if (!ctx)
        fail("cannot allocate template context");
    tmplr_err err = TMPLR_OK;
    if (key)
        err = tmplr_set_override(ctx, key, value);
    if (err == TMPLR_OK)
        err = tmplr_process_string(ctx, source, strlen(source), sink, out);
    tmplr_destroy(ctx);
    if (err != TMPLR_OK)
        fail("template expansion failed: %s", tmplr_strerror(err));
}

static bool
first_id(size_t index)
{
    for (size_t i = 0; i < index; i++)
        if (ids[i].chain == ids[index].chain && ids[i].id == ids[index].id)
            return false;
    return true;
}

static void
emit_registry(FILE *out, bool chain)
{
    const char *kind = chain ? "chain" : "event";
    fprintf(out,
            "DICE_HIDE bool dice_%s_valid_(%s_id id)\n{\n    switch (id) {\n",
            kind, chain ? "chain" : "type");
    for (size_t i = 0; i < nids; i++)
        if (ids[i].chain == chain && first_id(i) && (chain || ids[i].id))
            fprintf(out, "    case %d:\n", ids[i].id);
    fputs("        return true;\n    default: return false;\n    }\n}\n", out);
    fprintf(
        out,
        "DICE_HIDE const char *dice_%s_name_(%s_id id)\n{\n    switch (id) {\n",
        kind, chain ? "chain" : "type");
    for (size_t i = 0; i < nids; i++)
        if (ids[i].chain == chain && first_id(i))
            fprintf(out, "    case %d: return \"%s\";\n", ids[i].id,
                    ids[i].name);
    fputs("    default: return \"<unknown>\";\n    }\n}\n", out);
    /* ID-to-name switches select one canonical spelling. Reverse lookups
     * retain every declared spelling, including aliases for the same ID. */
    const char *type = chain ? "chain" : "type";
    fprintf(out,
            "DICE_HIDE %s_id dice_%s_lookup_(const char *name)\n{\n"
            "    static const struct { const char *name; %s_id id; } "
            "names[] = {\n",
            type, kind, type);
    for (size_t i = 0; i < nids; i++)
        if (ids[i].chain == chain)
            fprintf(out, "        {\"%s\", %d},\n", ids[i].name, ids[i].id);
    fputs(
        "    };\n"
        "    if (name)\n"
        "        for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); "
        "i++)\n"
        "            if (!strcmp(name, names[i].name))\n"
        "                return names[i].id;\n",
        out);
    fprintf(out, "    return MAX_%s;\n}\n", chain ? "CHAINS" : "TYPES");
}

static void
emit_sequence(FILE *out, int chain, int event)
{
    for (size_t i = 0; i < nsubs;) {
        int slot = subs[i].slot, current_chain = subs[i].chain;
        const char *specific = NULL, *wildcard = NULL;
        do {
            if (subs[i].event == 0)
                wildcard = subs[i].handler;
            else if (subs[i].event == event)
                specific = subs[i].handler;
            i++;
        } while (i < nsubs && subs[i].slot == slot &&
                 subs[i].chain == current_chain);
        if (current_chain != chain || (!specific && !wildcard))
            continue;
        render(out, source_text("call"), "$", "ROLL_HANDLER",
               specific ? specific : wildcard);
        fputs("        if (err == PS_STOP_CHAIN) return err;\n", out);
    }
    fputs("        return PS_OK;\n", out);
}

static void
emit_dispatch(FILE *out)
{
    fputs(
        "DICE_HIDE enum ps_err\nps_dispatch_(chain_id chain, type_id type, "
        "void *event, struct metadata *md)\n{\n"
        "    enum ps_err err = PS_OK;\n"
        "    (void)err; (void)type; (void)event; (void)md;\n"
        "    switch (chain) {\n",
        out);
    for (size_t c = 0; c < nids; c++) {
        if (!ids[c].chain || !first_id(c))
            continue;
        bool subscribed = false;
        for (size_t i = 0; i < nsubs; i++)
            if (subs[i].chain == ids[c].id)
                subscribed = true;
        if (!subscribed)
            continue;
        fprintf(out, "    case %d:\n        switch (type) {\n", ids[c].id);
        for (size_t e = 0; e < nids; e++) {
            if (ids[e].chain || !ids[e].id || !first_id(e))
                continue;
            bool exact = false;
            for (size_t i = 0; i < nsubs; i++)
                if (subs[i].chain == ids[c].id && subs[i].event == ids[e].id)
                    exact = true;
            if (!exact)
                continue;
            fprintf(out, "        case %d:\n", ids[e].id);
            emit_sequence(out, ids[c].id, ids[e].id);
        }
        fputs("        default:\n", out);
        emit_sequence(out, ids[c].id, 0);
        fputs("        }\n", out);
    }
    fputs("    default: return PS_OK;\n    }\n}\n", out);
}

static void
emit_route_expression(FILE *out, const struct subscription *routes,
                      size_t count, const char *slot, const char *chain,
                      const char *event)
{
    fputs("(0", out);
    for (size_t i = 0; i < count; i++) {
        fprintf(out, " || ((%s) == %d && (%s) == %d", slot, routes[i].slot,
                chain, routes[i].chain);
        if (routes[i].event)
            fprintf(out, " && (%s) == %d", event, routes[i].event);
        fputc(')', out);
    }
    fputc(')', out);
}

static void
emit_route_checks(FILE *out)
{
    fputs("#if DICE_CHECK_ROUTES\n", out);
    for (int publishes = 0; publishes < 2; publishes++) {
        fprintf(out,
                "DICE_HIDE bool dice_%s_allowed_(int slot, chain_id chain, "
                "type_id type)\n{\n"
                "    (void)slot; (void)chain; (void)type;\n    return ",
                publishes ? "publishes" : "subscribes");
        emit_route_expression(out, publishes ? prods : subs,
                              publishes ? nprods : nsubs, "slot", "chain",
                              "type");
        fputs(";\n}\n", out);
    }
    fputs("#endif\n", out);
}

static void
emit_preamble(FILE *out)
{
    fputs("/*\n", out);
    fputs(source_text("license"), out);
    fputs("*/\n\n", out);
    fprintf(out,
            "/*\n"
            " * Generated by dice-roll.\n"
            " * Configuration: %s\n"
            " * Edit the source configuration and regenerate this file.\n"
            " * Rebuild the runtime and plugins together.\n"
            " */\n",
            config_id);
}

static void emit_callback_index(FILE *out, bool chain);

static void
emit_header(FILE *out)
{
    emit_preamble(out);
    fputs(
        "#ifndef DICE_GENERATED_H\n#define DICE_GENERATED_H\n"
        "#if defined(DICE_MODULE_H) || defined(DICE_TYPES_H)\n"
        "#error Include the generated header before legacy Dice "
        "headers\n#endif\n"
        "#ifndef DICE_ROLLED\n#define DICE_ROLLED 1\n#endif\n"
        "#ifndef _GNU_SOURCE\n#define _GNU_SOURCE\n#endif\n",
        out);
    fprintf(out,
            "#ifndef DICE_CHECK_ROUTES\n#define DICE_CHECK_ROUTES 0\n#endif\n"
            "#if DICE_CHECK_ROUTES\n#define DICE_CONFIG_ID \"%s-checked\"\n"
            "#else\n#define DICE_CONFIG_ID \"%s\"\n#endif\n",
            config_id, config_id);
    fprintf(out, "#define DICE_CONFIG_HASH 0x%sULL\n", config_id + 7);
    fprintf(out,
            "#define DICE_CALLBACKS %d\n"
            "#define DICE_EXTENSION_CALLBACKS %d\n"
            "#define DICE_LAST_KNOWN_SLOT %d\n"
            "#define DICE_DEFAULT_PLUGIN_SLOT %d\n",
            callbacks, extension_callbacks, last_known_slot,
            last_known_slot == INT_MAX ? 0 :
            last_known_slot < 10000    ? 10000 :
                                         last_known_slot + 1);
    for (size_t i = 0; i < nslots; i++) {
        char name[NAME_SIZE];
        macro_name(name, slots[i].name);
        fprintf(out, "#define SLOT_%s %d\n", name, slots[i].number);
        fprintf(out, "#define DICE_STATIC_SLOT_%d 1\n", slots[i].number);
    }
    for (size_t i = 0; i < nsubs; i++)
        fprintf(out, "#define dice_subscribes_%d_%d_%d 1\n", subs[i].chain,
                subs[i].event, subs[i].slot);
    fputs("#if DICE_CHECK_ROUTES\n#define DICE_ROUTE_SUBSCRIBES(S, C, E) ",
          out);
    emit_route_expression(out, subs, nsubs, "S", "C", "E");
    fputs("\n#endif\n", out);
    int max_event = 127, max_chain = 6;
    for (size_t i = 0; i < nids; i++) {
        int *max = ids[i].chain ? &max_chain : &max_event;
        if (ids[i].id > *max)
            *max = ids[i].id;
        fprintf(out, "#define %s %d\n", ids[i].name, ids[i].id);
    }
    fprintf(out, "#define MAX_TYPES %d\n#define MAX_CHAINS %d\n", max_event + 1,
            max_chain + 1);
    fputs(source_text("public"), out);
    fputs("#ifdef DICE_ROLL_RUNTIME\n", out);
    fprintf(out, "#define MEMPOOL_SIZE UINT64_C(%" PRIu64 ")\n", mempool_size);
    fputs(
        "#if !defined(__APPLE__) && !defined(__NetBSD__)\n"
        "#define DICE_MEMPOOL_USE_MMAP\n#endif\n",
        out);
    if (callbacks) {
        emit_callback_index(out, true);
        emit_callback_index(out, false);
    }
    fputs("#endif\n", out);

    fputs(source_text("module"), out);
    for (size_t i = 0; i < nsubs; i++)
        fprintf(out,
                "DICE_HIDE enum ps_err %s(chain_id, type_id, void *, "
                "struct metadata "
                "*);\n",
                subs[i].handler);
    fputs("#endif /* DICE_GENERATED_H */\n", out);
}

static void
emit_callback_index(FILE *out, bool chain)
{
    const char *kind = chain ? "CHAIN" : "TYPE";
    int maximum = 0, count = 0;
    for (size_t i = 0; i < nids; i++) {
        if (ids[i].chain != chain || !first_id(i))
            continue;
        count++;
        if (ids[i].id > maximum)
            maximum = ids[i].id;
    }
    /* Preserve direct indexing for small/dense IDs; compact large forced IDs
     * so one high event and chain cannot create a multi-gigabyte matrix. */
    if (maximum < (chain ? 64 : 256) || maximum < count * 2) {
        fprintf(out, "#define PS_%s_COUNT %d\n#define PS_%s_INDEX(ID) (ID)\n",
                kind, maximum + 1, kind);
        return;
    }
    fprintf(out, "#define PS_%s_COUNT %d\n", kind, count);
    fprintf(out,
            "static inline size_t ps_%s_index_(unsigned id)\n{\n"
            "    switch (id) {\n",
            kind);
    int index = chain ? 0 : 1;
    for (size_t i = 0; i < nids; i++) {
        if (ids[i].chain != chain || !first_id(i))
            continue;
        int mapped = !chain && !ids[i].id ? 0 : index++;
        fprintf(out, "    case %d: return %d;\n", ids[i].id, mapped);
    }
    fprintf(out,
            "    default: return PS_%s_COUNT;\n    }\n}\n"
            "#define PS_%s_INDEX(ID) ps_%s_index_(ID)\n",
            kind, kind, kind);
}

static void
emit_callback_call(FILE *out, int chain, int slot, const char *index)
{
    fprintf(out,
            "    err = ps_callback_invoke_(dice_callbacks_%d_%d[%s].head, "
            "chain, type, event, md);\n"
            "    if (err == PS_STOP_CHAIN) return err;\n",
            chain, slot, index);
}

static void
emit_known_callbacks(FILE *out)
{
    /* Weak definitions belong only in dice.c: putting the weak attribute on
     * header prototypes would also weaken the linked module definitions. */
    for (size_t i = 0; i < nsubs;) {
        size_t first = i;
        int chain = subs[i].chain, slot = subs[i].slot;
        do {
            i++;
        } while (i < nsubs && subs[i].chain == chain && subs[i].slot == slot);
        if (!subs[first].callbacks)
            continue;
        fprintf(out,
                "static struct type dice_callbacks_%d_%d[PS_TYPE_COUNT];\n",
                chain, slot);
        for (size_t j = first; j < i; j++) {
            fprintf(out,
                    "DICE_WEAK DICE_HIDE bool dice_bound_%d_%d_%d(void)\n"
                    "{ return false; }\n",
                    chain, subs[j].event, slot);
            fprintf(out,
                    "DICE_WEAK DICE_HIDE enum ps_err %s(chain_id chain, "
                    "type_id type, void *event, struct metadata *md)\n{\n"
                    "    (void)chain; (void)type; (void)event; (void)md;\n"
                    "    size_t index = PS_TYPE_INDEX(type);\n"
                    "    if (!index || index >= PS_TYPE_COUNT) return "
                    "PS_HANDLER_OFF;\n"
                    "    enum ps_err err;\n",
                    subs[j].handler);
            if (!subs[j].event) {
                /* Explicit cases never invoke the wildcard handler. This
                 * handler also serves dynamically registered callbacks for
                 * events without an explicit case in this slot. */
                fputs("    if (1", out);
                for (size_t k = first; k < i; k++)
                    if (subs[k].event)
                        fprintf(out, " && type != %d", subs[k].event);
                fputs(") {\n", out);
                emit_callback_call(out, chain, slot, "index");
                fputs("    }\n", out);
                emit_callback_call(out, chain, slot, "0");
            } else {
                emit_callback_call(out, chain, slot, "index");
            }
            fputs("    return PS_HANDLER_OFF;\n}\n", out);
        }
    }
    fputs(
        "DICE_HIDE int dice_subscribe_known_(chain_id chain, type_id type, "
        "ps_callback_f callback, int slot)\n{\n"
        "    (void)chain; (void)type; (void)callback; (void)slot;\n",
        out);
    for (size_t i = 0; i < nsubs;) {
        size_t first = i;
        int chain = subs[i].chain, slot = subs[i].slot;
        bool wildcard = false;
        do {
            wildcard |= !subs[i].event;
            i++;
        } while (i < nsubs && subs[i].chain == chain && subs[i].slot == slot);
        if (!subs[first].callbacks)
            continue;
        fprintf(out, "    if (slot == %d && chain == %d) {\n", slot, chain);
        if (!wildcard) {
            fputs("        if (!(0", out);
            for (size_t j = first; j < i; j++)
                fprintf(out, " || type == %d", subs[j].event);
            fputs(")) return PS_INVALID;\n", out);
        }
        fprintf(out, "        struct type *heads = dice_callbacks_%d_%d;\n",
                chain, slot);
        fputs(
            "        size_t index = PS_TYPE_INDEX(type);\n"
            "        if (index >= PS_TYPE_COUNT) return PS_INVALID;\n"
            "        if (heads[index].head) return PS_INVALID;\n",
            out);
        /* Match the dispatcher's exact-event precedence. A linked wildcard
         * also occupies events without their own declared case. */
        fputs("        switch (type) {\n", out);
        for (size_t j = first; j < i; j++) {
            if (!subs[j].event)
                continue;
            fprintf(out,
                    "        case %d:\n"
                    "            if (dice_bound_%d_%d_%d()) return "
                    "PS_INVALID;\n"
                    "            break;\n",
                    subs[j].event, chain, subs[j].event, slot);
        }
        fputs("        default:\n", out);
        if (wildcard)
            fprintf(out,
                    "            if (dice_bound_%d_0_%d()) return "
                    "PS_INVALID;\n",
                    chain, slot);
        fputs(
            "            break;\n        }\n"
            "        return ps_subscribe_list_(&heads[index], chain, type, "
            "callback, slot, type == ANY_EVENT);\n    }\n",
            out);
    }
    fputs("    return PS_INVALID;\n}\n", out);
}

static void
emit_event_ids(FILE *out)
{
    fputs("DICE_HIDE const type_id ps_event_ids_[] = {\n", out);
    for (size_t i = 0; i < nids; i++)
        if (!ids[i].chain && ids[i].id && first_id(i))
            fprintf(out, "    %d,\n", ids[i].id);
    fputs(
        "};\nDICE_HIDE const size_t ps_event_count_ = "
        "sizeof(ps_event_ids_) / sizeof(ps_event_ids_[0]);\n",
        out);
}

static void
emit_source(FILE *out, const char *name)
{
    emit_preamble(out);
    fprintf(out, "#define DICE_ROLL_RUNTIME\n#include \"%s.h\"\n", name);
    fputs("#include \"pubsub-internal.h\"\n#include <string.h>\n", out);
    fprintf(
        out,
        "_Static_assert(DICE_CONFIG_HASH == 0x%sULL, "
        "\"generated source and header are from different configurations\");\n",
        config_id + 7);
    emit_route_checks(out);
    emit_registry(out, false);
    emit_registry(out, true);
    if (extension_callbacks)
        emit_event_ids(out);
    if (callbacks)
        emit_known_callbacks(out);
    emit_dispatch(out);
}

static void
replace_output(const char *tmp_c, const char *tmp_h, const char *dest_c,
               const char *dest_h)
{
    char backup_dir[PATH_MAX], backup_h[PATH_MAX];
    bool have_backup = false, installed_header = false;
    bool backup_created = false;
    int error;
    if (snprintf(backup_dir, sizeof(backup_dir), "%s.backup.XXXXXX", tmp_h) >=
            (int)sizeof(backup_dir) ||
        snprintf(backup_h, sizeof(backup_h), "%s/header", backup_dir) >=
            (int)sizeof(backup_h)) {
        errno = ENAMETOOLONG;
        goto failure;
    }
    if (!mkdtemp(backup_dir))
        goto failure;
    backup_created = true;
    /* mkdtemp replaced the suffix, without changing the path's length. */
    memcpy(backup_h, backup_dir, strlen(backup_dir));

    struct stat st;
    if (lstat(dest_h, &st) == 0) {
        if (!S_ISREG(st.st_mode) && !S_ISLNK(st.st_mode)) {
            errno = EINVAL;
            goto failure;
        }
        if (rename(dest_h, backup_h))
            goto failure;
        have_backup = true;
    } else if (errno != ENOENT) {
        goto failure;
    }
    if (rename(tmp_h, dest_h))
        goto failure;
    installed_header = true;
    /* Source replacement is the final commit step. A failed rename leaves
     * the previous source intact, so only the header needs rolling back. */
    if (rename(tmp_c, dest_c))
        goto failure;
    if (have_backup && unlink(backup_h))
        fprintf(stderr, "dice: cannot remove backup %s: %s\n", backup_h,
                strerror(errno));
    else if (rmdir(backup_dir))
        fprintf(stderr, "dice: cannot remove backup directory %s: %s\n",
                backup_dir, strerror(errno));
    return;

failure:
    error = errno;
    unlink(tmp_c);
    unlink(tmp_h);
    if (have_backup) {
        if (rename(backup_h, dest_h))
            fail(
                "cannot replace output (%s) or restore header (%s); "
                "previous header saved at %s",
                strerror(error), strerror(errno), backup_h);
    } else if (installed_header && unlink(dest_h)) {
        fail("cannot replace output (%s) or remove new header %s (%s)",
             strerror(error), dest_h, strerror(errno));
    }
    if (backup_created)
        rmdir(backup_dir);
    fail("cannot replace output: %s", strerror(error));
}

static void
dependency_path(FILE *out, const char *path)
{
    for (; *path; path++) {
        if (*path == '\n' || *path == '\r')
            fail("dependency paths cannot contain newlines");
        if (*path == '$')
            fputc('$', out);
        else if (strchr(" \t#:\\", *path))
            fputc('\\', out);
        fputc(*path, out);
    }
}

static void
output_path(const char *path, char absolute[PATH_MAX])
{
    char copy[PATH_MAX], parent[PATH_MAX];
    if (snprintf(copy, sizeof(copy), "%s", path) >= (int)sizeof(copy))
        fail("output path is too long");
    char *name            = strrchr(copy, '/');
    const char *directory = ".";
    if (name) {
        *name++   = '\0';
        directory = *copy ? copy : "/";
    } else {
        name = copy;
    }
    if (!realpath(directory, parent))
        fail("%s: %s", directory, strerror(errno));
    if (snprintf(absolute, PATH_MAX, "%s/%s", parent, name) >= PATH_MAX)
        fail("output path is too long");
}

static void
write_dependencies(const char *path, const char *source, const char *header)
{
    char destination[PATH_MAX], other[PATH_MAX];
    output_path(path, destination);
    output_path(source, other);
    if (!strcmp(destination, other))
        fail("dependency file must differ from generated output files");
    output_path(header, other);
    if (!strcmp(destination, other))
        fail("dependency file must differ from generated output files");
    for (size_t i = 0; i < ninputs; i++)
        if (!strcmp(destination, input_paths[i]))
            fail("dependency file must differ from configuration inputs");
    for (size_t i = 0; i < COUNT(sources); i++) {
        if (!realpath(sources[i].path, other))
            fail("%s: %s", sources[i].path, strerror(errno));
        if (!strcmp(destination, other))
            fail("dependency file must differ from generator sources");
    }
    char tmp[PATH_MAX];
    if (snprintf(tmp, sizeof(tmp), "%s.XXXXXX", path) >= (int)sizeof(tmp))
        fail("dependency file path is too long");
    int fd = mkstemp(tmp);
    if (fd < 0)
        fail("cannot create dependency file %s: %s", path, strerror(errno));
    FILE *out = fdopen(fd, "w");
    if (!out) {
        close(fd);
        unlink(tmp);
        fail("cannot open dependency file %s", path);
    }
    dependency_path(out, source);
    fputc(' ', out);
    dependency_path(out, header);
    fputc(':', out);
    for (size_t i = 0; i < ninputs; i++) {
        fputc(' ', out);
        dependency_path(out, input_paths[i]);
    }
    for (size_t i = 0; i < COUNT(sources); i++) {
        fputc(' ', out);
        dependency_path(out, sources[i].path);
    }
    fputc('\n', out);
    bool error = ferror(out);
    if (fclose(out))
        error = true;
    if (error || rename(tmp, path)) {
        unlink(tmp);
        fail("cannot write dependency file %s: %s", path, strerror(errno));
    }
}

static void
generate(const char *directory, const char *name, const char *depfile)
{
    if (!*name || strlen(name) >= NAME_SIZE)
        fail("output name must contain 1..%d characters", NAME_SIZE - 1);
    for (const char *p = name; *p; p++)
        if (!(isalnum((unsigned char)*p) || *p == '_' || *p == '-'))
            fail("output name must use only letters, digits, '_' or '-'");
    if (mkdir(directory, 0755) && errno != EEXIST)
        fail("%s: %s", directory, strerror(errno));
    char tmp_c[PATH_MAX], tmp_h[PATH_MAX], dest_c[PATH_MAX], dest_h[PATH_MAX];
    if (snprintf(dest_c, sizeof(dest_c), "%s/%s.c", directory, name) >=
            (int)sizeof(dest_c) ||
        snprintf(dest_h, sizeof(dest_h), "%s/%s.h", directory, name) >=
            (int)sizeof(dest_h) ||
        snprintf(tmp_c, sizeof(tmp_c), "%s/.%s.c.XXXXXX", directory, name) >=
            (int)sizeof(tmp_c) ||
        snprintf(tmp_h, sizeof(tmp_h), "%s/.%s.h.XXXXXX", directory, name) >=
            (int)sizeof(tmp_h))
        fail("output path is too long");
    if (depfile)
        write_dependencies(depfile, dest_c, dest_h);
    int fd_c = mkstemp(tmp_c), fd_h = mkstemp(tmp_h);
    if (fd_c < 0 || fd_h < 0)
        fail("cannot create output: %s", strerror(errno));
    FILE *c = fdopen(fd_c, "w"), *h = fdopen(fd_h, "w");
    if (!c || !h)
        fail("cannot open output streams: %s", strerror(errno));
    emit_header(h);
    emit_source(c, name);
    bool error = ferror(c) || ferror(h);
    if (fclose(c))
        error = true;
    if (fclose(h))
        error = true;
    if (error || chmod(tmp_c, 0644) || chmod(tmp_h, 0644)) {
        unlink(tmp_c);
        unlink(tmp_h);
        fail("cannot write output: %s", strerror(errno));
    }
    replace_output(tmp_c, tmp_h, dest_c, dest_h);
}

static void
usage(FILE *out)
{
    fputs(
        "Usage: dice-roll [-I DIRECTORY ...] CONFIG.dice --output DIRECTORY\n"
        "                 [--name NAME] [--depfile FILE]\n"
        "       dice-roll [-I DIRECTORY ...] --check CONFIG.dice\n"
        "       dice-roll [-I DIRECTORY ...] --query CONFIG.dice\n"
        "       dice-roll [-I DIRECTORY ...] --graph CONFIG.dice\n"
        "Imports search -I directories in order, then " DICE_SOURCE_DIR
        "/src.\n"
        "Use -I NAMESPACE=DIRECTORY to mount a namespace (e.g. "
        "-Idice=src/mod).\n"
        "The default dice namespace also searches " DICE_SOURCE_DIR
        "/src/mod.\n",
        out);
}

int
main(int argc, char **argv)
{
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
        usage(stdout);
        return 0;
    }
    if (argc == 2 && !strcmp(argv[1], "--version")) {
        puts("dice-roll 1 (configuration schema 1)");
        return 0;
    }
    if (argc == 2 && !strcmp(argv[1], "--licenses")) {
        puts(source_text("license"));
        return 0;
    }
    if (argc < 2) {
        usage(stderr);
        return 1;
    }
    enum { ROLL, CHECK, QUERY, GRAPH } mode = ROLL;
    const char *config                      = NULL;
    const char *directory = NULL, *name = NULL, *depfile = NULL;
    for (int i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "--check") || !strcmp(argv[i], "--query") ||
             !strcmp(argv[i], "--graph")) &&
            mode == ROLL) {
            mode = !strcmp(argv[i], "--check") ? CHECK :
                   !strcmp(argv[i], "--query") ? QUERY :
                                                 GRAPH;
        } else if (!strncmp(argv[i], "-I", 2) && (argv[i][2] || i + 1 < argc)) {
            const char *path = argv[i][2] ? argv[i] + 2 : argv[++i];
            add_import_path(path);
        } else if ((!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) &&
                   !directory && i + 1 < argc) {
            directory = argv[++i];
        } else if (!strcmp(argv[i], "--name") && !name && i + 1 < argc) {
            name = argv[++i];
        } else if (!strcmp(argv[i], "--depfile") && !depfile && i + 1 < argc) {
            depfile = argv[++i];
        } else if (argv[i][0] != '-' && !config) {
            config = argv[i];
        } else {
            usage(stderr);
            return 1;
        }
    }
    if (!config || (mode == ROLL ? !directory : directory || name || depfile)) {
        usage(stderr);
        return 1;
    }
    input_name = config;
    add_import_path(DICE_SOURCE_DIR "/src");
    add_import_path("dice=" DICE_SOURCE_DIR "/src/mod");
    for (size_t i = 0; i < COUNT(standard_ids); i++)
        add_id(standard_ids[i].name, standard_ids[i].id, standard_ids[i].chain,
               true);
    const char *ancestors[32];
    read_config(input_name, ancestors, 0, false, "", false);
    resolve();
    if (mode == ROLL)
        generate(directory, name ? name : "dice", depfile);
    else if (mode == QUERY) {
        printf("configuration %s\ncallbacks %s\n", config_id,
               extension_callbacks ? "true" : "false");
        for (size_t i = 0; i < nids; i++)
            printf("%s %s %d\n", ids[i].chain ? "chain" : "event", ids[i].name,
                   ids[i].id);
        for (size_t i = 0; i < nslots; i++)
            printf("slot %s %d%s callbacks %s\n", slots[i].name,
                   slots[i].number, slots[i].forced >= 0 ? " forced" : "",
                   callbacks_allowed(slots[i].name) ? "true" : "false");
        for (size_t i = 0; i < nsubs; i++)
            printf("subscribes %s %d/%d\n", subs[i].slot_name, subs[i].chain,
                   subs[i].event);
        for (size_t i = 0; i < nprods; i++)
            printf("publishes %s %d/%d\n", prods[i].slot_name, prods[i].chain,
                   prods[i].event);
    } else if (mode == GRAPH) {
        puts("digraph dice {");
        for (size_t i = 0; i < nslots; i++) {
            printf("  \"slot:%s\" [label=\"%s (%d)\"];\n", slots[i].name,
                   slots[i].name, slots[i].number);
            for (size_t j = 0; j < nslots; j++)
                if (edges[i][j])
                    printf(
                        "  \"slot:%s\" -> \"slot:%s\" "
                        "[style=dashed,label=\"before\"];\n",
                        slots[i].name, slots[j].name);
        }
        for (size_t i = 0; i < nsubs; i++)
            printf("  \"chain:%d\" -> \"slot:%s\" [label=\"event %d\"];\n",
                   subs[i].chain, subs[i].slot_name, subs[i].event);
        for (size_t i = 0; i < nprods; i++)
            printf("  \"slot:%s\" -> \"chain:%d\" [label=\"event %d\"];\n",
                   prods[i].slot_name, prods[i].chain, prods[i].event);
        puts("}");
    }
    return 0;
}
