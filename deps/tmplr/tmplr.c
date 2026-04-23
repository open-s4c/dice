/*
 * Copyright (C) 2022-2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 *
 * tmplr - a template replacement tool
 *
 * tmplr is a simple tool to achieve a minimum level of genericity without
 * resorting to C preprocessor macros.
 *
 */

#if defined(__linux__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#elif defined(__APPLE__) && !defined(_DARWIN_C_SOURCE)
    #define _DARWIN_C_SOURCE
#else
    #ifndef _XOPEN_SOURCE
        #define _XOPEN_SOURCE 700
    #endif
#endif

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tmplr.h>
#include "version.h"

/*******************************************************************************
 * Logging
 ******************************************************************************/

static bool _verbose;
static jmp_buf *trap_env;
static tmplr_err trap_err = TMPLR_OK;
static tmplr_sink_fn active_sink;
static void *active_sink_user;

/* maximum length of a value */
static size_t max_vlen = 256;

static int
default_sink(const char *buf, size_t len, void *user)
{
    FILE *out = (FILE *)user;
    if (out == NULL)
        out = stdout;
    return fwrite(buf, 1, len, out) == len ? 0 : -1;
}

static void
dief(tmplr_err err, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    trap_err = err;
    if (trap_env != NULL)
        longjmp(*trap_env, 1);
    exit(EXIT_FAILURE);
}

static void
emit_buf(const char *buf, size_t len)
{
    tmplr_sink_fn sink = active_sink ? active_sink : default_sink;
    void *user         = active_sink ? active_sink_user : stdout;
    if (sink(buf, len, user) != 0)
        dief(TMPLR_ERR_IO, "error: output sink failed\n");
}

static void
debugf(const char *fmt, ...)
{
    if (!_verbose)
        return;
    va_list ap;
    va_start(ap, fmt);
    emit_buf("// ", 3);
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int need = vsnprintf(NULL, 0, fmt, ap_copy);
    va_end(ap_copy);
    if (need >= 0) {
        char *buf = calloc((size_t)need + 1, 1);
        if (buf != NULL) {
            vsnprintf(buf, (size_t)need + 1, fmt, ap);
            emit_buf(buf, (size_t)need);
            free(buf);
        }
    }
    va_end(ap);
}

/*******************************************************************************
 * Maximum line lengths and buffer sizes
 ******************************************************************************/

/* maximum length of a line */
#ifndef MAX_SLEN
    #define MAX_SLEN 256UL
#endif
/* maximum number of lines in a block */
#ifndef MAX_BLEN
    #define MAX_BLEN 100
#endif
/* maximum number of keys */
#ifndef MAX_KEYS
    #define MAX_KEYS 1024
#endif
/* maximum length of a key */
#ifndef MAX_KLEN
    #define MAX_KLEN 64
#endif
/* Buffer to hold the key */
#define K_BUF_LEN ((MAX_KLEN) + 1)
/* maximum number of replacements per line */
#ifndef MAX_APPLY
    #define MAX_APPLY 32
#endif
/* absolute upper bound for max_vlen */
#ifndef ABSOLUTE_MAX_VLEN
    #define ABSOLUTE_MAX_VLEN (4096UL)
#endif
/* minimum buffer length to hold an int*/
#ifndef V_MIN_LEN
    #define V_MIN_LEN 32
#endif

/*******************************************************************************
 * Template commands
 ******************************************************************************/
#define TMPL_PREFIX        "$"
#define TMPL_SUFFIX_MAP    "_map"
#define TMPL_SUFFIX_BEGIN  "_begin("
#define TMPL_SUFFIX_END    "_end"
#define TMPL_SUFFIX_MUTE   "_mute"
#define TMPL_SUFFIX_UNMUTE "_unmute"
#define TMPL_SUFFIX_ABORT  "_abort"
#define TMPL_SUFFIX_SKIP   "_skip"
#define TMPL_SUFFIX_KILL   "_kill"
#define TMPL_SUFFIX_UNDO   "_undo"
#define TMPL_SUFFIX_DL     "_dl"
#define TMPL_SUFFIX_NL     "_nl"
#define TMPL_SUFFIX_UPCASE "_upcase"
#define TMPL_SUFFIX_HOOK   "_hook"
#define TMPL_SUFFIX_ICOUNT "_icount"
#define TMPL_SUFFIX_ISLAST "_islast"

static char *TMPL_MAP;
static char *TMPL_BEGIN;
static char *TMPL_END;
static char *TMPL_MUTE;
static char *TMPL_UNMUTE;
static char *TMPL_ABORT;
static char *TMPL_SKIP;
static char *TMPL_KILL;
static char *TMPL_UNDO;
static char *TMPL_DL;
static char *TMPL_NL;
static char *TMPL_UPCASE;
static char *TMPL_HOOK;
static char *TMPL_ICOUNT;
static char *TMPL_ISLAST;

/* set prefix of commands. If prefix is NULL, use default TMPL_PREFIX. */
void
set_prefix(const char *prefix)
{
    if (prefix == NULL) {
        prefix = TMPL_PREFIX;
    }

#define CMD_PAIR(X)                                                            \
    {                                                                          \
        .cmd = &TMPL_##X, .suffix = TMPL_SUFFIX_##X                            \
    }
    struct {
        char **cmd;
        const char *suffix;
    } cmds[] = {
        CMD_PAIR(MAP),    CMD_PAIR(BEGIN),  CMD_PAIR(END),    CMD_PAIR(MUTE),
        CMD_PAIR(UNMUTE), CMD_PAIR(ABORT),  CMD_PAIR(SKIP),   CMD_PAIR(KILL),
        CMD_PAIR(UNDO),   CMD_PAIR(DL),     CMD_PAIR(NL),     CMD_PAIR(UPCASE),
        CMD_PAIR(HOOK),   CMD_PAIR(ICOUNT), CMD_PAIR(ISLAST), {NULL, NULL},
    };

    for (int i = 0; cmds[i].cmd != NULL; i++) {
        const char *suffix = cmds[i].suffix;
        *cmds[i].cmd       = calloc(strlen(suffix) + strlen(prefix) + 1, 1);
        if (*cmds[i].cmd == NULL) {
            dief(TMPLR_ERR_OOM, "error: could not allocate command\n");
        }
        strcat(*cmds[i].cmd, prefix);
        strcat(*cmds[i].cmd, suffix);
        debugf("[CMD] %s\n", *cmds[i].cmd);
    }
}

/*******************************************************************************
 * Type definitions
 ******************************************************************************/

/* pair_t is a key-value pair. Key and value are 0-terminated char arrays.
 */
typedef struct {
    char key[K_BUF_LEN];
    char *val;
} pair_t;

/* err_t represents an error message */
typedef struct {
    const char *msg;
} err_t;

#define NO_ERROR                                                               \
    (err_t)                                                                    \
    {                                                                          \
        0                                                                      \
    }
#define ERROR(m)                                                               \
    (err_t)                                                                    \
    {                                                                          \
        .msg = m                                                               \
    }
#define IS_ERROR(err) (err).msg != NULL

typedef struct {
    char *key;
    char *val;
} map_entry_t;

struct tmplr_ctx {
    tmplr_opts opts;
    map_entry_t *override_entries;
    size_t override_len;
    size_t override_cap;
    map_entry_t *filter_entries;
    size_t filter_len;
    size_t filter_cap;
};

/*******************************************************************************
 * String functions
 ******************************************************************************/

void
trim(char *s, char c)
{
    assert(s);

    size_t len = strlen(s);

    /* remove trailing space */
    for (; len > 0 && s[len - 1] == c; len = strlen(s))
        s[len - 1] = '\0';

    /* remove leading space */
    while (s[0] == c)
        /* use len of s to include \0 */
        memmove(s, s + 1, strlen(s));
}

void
trims(char *s, char *chars)
{
    for (char *c = chars; *c; c++)
        trim(s, *c);
}

/*******************************************************************************
 * Mappings
 ******************************************************************************/

/* template mappings: key -> value lists
 *
 * Passed as arguments to TMPL_BEGIN.
 */
pair_t template_map[MAX_KEYS];

/* template override mappings : key -> value
 *
 * Given via command line -D option. These overwrite template mapping values
 */
pair_t override_map[MAX_KEYS];

/* template filter mappings : key -> value
 *
 * Given via command line -F option. These filter template mapping values
 */
pair_t filter_map[MAX_KEYS];

/* iteration mappings: key -> value
 *
 * These are the single values of the template mappings, potentially
 * overriden by override_map or filtered by filter_map. They are set at
 * each iteration of a template block.
 *
 * They precede the persistent mappings.
 */
pair_t iteration_map[MAX_KEYS];

/* persistent mappings: key -> value
 *
 * Given via TMPL_MAP(key, value) commands outside of template blocks.
 *
 * These succeed the iteration mappings.
 */
pair_t persistent_map[MAX_KEYS];

/* block hooks */
pair_t block_hooks[MAX_KEYS];

void
remap(pair_t *map, const char *key, const char *val)
{
    if (key == NULL)
        return;
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (!p->key[0] || strcmp(p->key, key) == 0) {
            memset(p->key, 0, sizeof(p->key));
            strncat(p->key, key, sizeof(p->key) - 1);
            trims(p->key, " \t");

            size_t input_len = strlen(val);
            if (input_len > max_vlen) {
                dief(TMPLR_ERR_LIMIT,
                     "error: value for key '%s' is too long (%zu > %zu)\n", key,
                     input_len, max_vlen);
            }

            if (p->val != NULL) {
                free(p->val);
                p->val = NULL;
            }

            p->val = strdup(val);

            trims(p->val, " ");

            debugf("[REMAP] %s = %s\n", p->key, p->val);
            return;
        }
    }
}

pair_t *
find(pair_t *map, const char *key)
{
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (strcmp(p->key, key) == 0)
            return p;
    }
    return NULL;
}

void
unmap(pair_t *map, char *key)
{
    pair_t *p = find(map, key);
    if (p == NULL)
        return;

    memset(p->key, 0, MAX_KLEN);

    if (p->val != NULL) {
        free(p->val);
        p->val = NULL;
    }
}

void
show(pair_t *map, const char *name)
{
    debugf("[SHOW MAP] %s\n", name);
    for (int i = 0; i < MAX_KEYS; i++) {
        pair_t *p = map + i;
        if (p->key[0]) {
            debugf("\t%s = %s\n", p->key, p->val);
        }
    }
}

void
clean(pair_t *map)
{
    for (int i = 0; i < MAX_KEYS; i++) {
        if (map[i].val != NULL) {
            free(map[i].val);
            map[i].val = NULL;
        }
    }
    memset(map, 0, sizeof(pair_t) * MAX_KEYS);
}

/*******************************************************************************
 * options
 ******************************************************************************/

enum options { OPT_KV_SEP = 1, OPT_ITEM_SEP = 2, OPT_ITER_SEP = 3 };
static struct {
    char *val;
} options[] = {[OPT_KV_SEP]   = {.val = ","},
               [OPT_ITEM_SEP] = {.val = ","},
               [OPT_ITER_SEP] = {.val = ";"}};
#define OPTION(KEY) (options[OPT_##KEY].val)

err_t
set_option(enum options opt, char *val)
{
    options[opt].val = strdup(val);
    return NO_ERROR;
}

/*******************************************************************************
 * parse functions
 ******************************************************************************/

#define goto_error(label, error)                                               \
    {                                                                          \
        err = ERROR(error);                                                    \
        goto label;                                                            \
    }
err_t
parse_assign(pair_t *p, char *start, char *end)
{
    char key[K_BUF_LEN + 1] = {0};

    err_t err = NO_ERROR;

    char *val = calloc(max_vlen + 1, sizeof(char));
    if (!val) {
        goto_error(cleanup, "out of memory");
    }

    char *comma = strstr(start, OPTION(KV_SEP));
    if (comma == NULL) {
        goto_error(cleanup, "expected separator");
    }
    start++;
    if (comma - start >= K_BUF_LEN) {
        goto_error(cleanup, "key is too long");
    }
    strncat(key, start, comma - start);
    comma++;
    if ((size_t)(end - comma) >= max_vlen) {
        goto_error(cleanup, "value is too long");
    }
    strncat(val, comma, end - comma);
    remap(p, key, val);

cleanup:
    free(val);
    return err;
}

err_t
parse_template_map(char *start, char *end)
{
    char *next, *values;
    start++;
    *end = '\0';

    char *val = calloc(max_vlen + 1, sizeof(char));
    if (!val)
        return ERROR("out of memory");

again:
    next = strstr(start, OPTION(ITEM_SEP));
    if (next) {
        *next = '\0';
        next++;
    }
    values = strstr(start, "=");
    if (values == NULL) {
        free(val);
        return ERROR("expected '='");
    }
    *values = '\0';
    values++;

    char key[K_BUF_LEN] = {0};
    strncat(key, start, MAX_KLEN);

    /* clear the buffer to ensure proper NUL-termination and garbage data */
    memset(val, 0, max_vlen + 1);

    size_t src_len = strlen(values);
    if (src_len < max_vlen)
        strcat(val, values);
    else
        strncat(val, values, max_vlen);

    trims(val, " []");

    remap(template_map, key, val);

    if (next) {
        start = next;
        goto again;
    }

    free(val);
    return NO_ERROR;
}

/*******************************************************************************
 * Line-based processing
 ******************************************************************************/
bool muted = false;

void
line_add_nl(char *line)
{
    const size_t len = strlen(line);
    if (line[len - 1] != '\n') {
        assert(len + 1 < MAX_SLEN);
        line[len]     = '\n';
        line[len + 1] = '\0';
    }
}

bool
line_apply(char *line, const char *key, const char *val)
{
    char *cur;

    if (!key[0] || (cur = strstr(line, key)) == NULL)
        return false;

    const size_t vlen = strlen(val);
    const size_t klen = strlen(key);

    const bool is_nl = strcmp(key, TMPL_NL) == 0;
    if (!is_nl)
        debugf("[APPLY] KEY: %s(%lu) VAL: %s(%lu)\n", key, klen, val, vlen);

    /* make space for value */
    if (!is_nl)
        debugf("\tBEFORE: %s", line);

    /* prefix length */
    const size_t prefix = (size_t)(cur - line);
    /* tail length */
    const size_t tlen = strlen(cur + klen) + 1;

    if (prefix + vlen + tlen > MAX_SLEN) {
        dief(TMPLR_ERR_LIMIT, "error: cannot apply beyond line limit (%lu)\n",
             MAX_SLEN);
    }

    memmove(cur + vlen, cur + klen, tlen);
    memcpy(cur, val, vlen);
    if (!is_nl)
        debugf("\tAFTER:  %s", line);

    return true;
}

bool
process_block_line(char *line)
{
    bool applied;
    char *cur;

    char buf[MAX_SLEN] = {0};
    strcat(buf, line);
    line_add_nl(buf);

    debugf("[LINE] %s", buf);
    int cnt = 0;
again:
    applied = false;

    for (int i = 0; i < MAX_KEYS && cnt < MAX_APPLY; i++) {
        /* should delete line? */
        if (strstr(buf, TMPL_DL)) {
            strcpy(buf, "");
            goto end;
        }
        if (strstr(buf, TMPL_SKIP))
            return false;
        if ((cur = strstr(buf, TMPL_KILL)))
            *cur = '\0';
        if ((cur = strstr(buf, TMPL_UNDO))) {
            size_t skip = strlen(TMPL_UNDO);
            size_t len  = strlen(cur);
            size_t rlen = len - skip + 1;
            if (rlen > MAX_SLEN) {
                dief(TMPLR_ERR_LIMIT,
                     "error: line longer than limit (%lu > %lu)\n", rlen,
                     MAX_SLEN);
            }
            memmove(buf, cur + skip, rlen);
        }

        const pair_t *pi = iteration_map + i;
        const pair_t *pp = persistent_map + i;
        if (!line_apply(buf, pi->key, pi->val) &&
            !line_apply(buf, pp->key, pp->val))
            continue;
        applied = true;
        cnt++;

        /* if one mapping is applied, restart testing all mappings */
        i = -1;
    }
    if (applied)
        goto again;

end:

    /* apply UPCASE */
    while ((cur = strstr(buf, TMPL_UPCASE))) {
        char *start = cur + strlen(TMPL_UPCASE);
        char sep    = start[0] == '(' ? ')' : start[0];
        char ssep[] = {sep, 0};
        char *end   = strstr(start + 1, ssep);
        assert(start && end && end > start);
        char *ch = start + 1;
        while (ch < end) {
            if (*ch >= 'a' && *ch <= 'z')
                *ch -= ('a' - 'A');
            ch++;
        }
        size_t len = (end - start) - 1;
        memmove(cur, start + 1, len);
        /* include the end of line */
        memmove(cur + len, end + 1, strlen(end));
    }

    /* apply NL */
    while (line_apply(buf, TMPL_NL, "\n"))
        ;

    /* output and return */
    emit_buf(buf, strlen(buf));
    if (cnt >= MAX_APPLY) {
        dief(TMPLR_ERR_LIMIT, "error: too many replacements (%d)\n", cnt);
    }
    return true;
}

/*******************************************************************************
 * Block processing
 ******************************************************************************/

void
process_begin(void)
{
    debugf("============================\n");
    debugf("[BLOCK_BEGIN]\n");
    show(persistent_map, "persistent_map");
    show(block_hooks, "block_hooks");
    show(template_map, "template_map");
    debugf("----------------------------\n");
}

/*******************************************************************************
 * Block buffer
 *
 * Inside a template block, we buffer the whole block and then output the
 * content of hte buffer with mappings applied for each value of the mapping
 * iterators.
 ******************************************************************************/
static size_t max_block_lines = MAX_BLEN;
static char (*save_block)[MAX_SLEN];
static size_t save_k;

static void
allocate_block_buffer(void)
{
    if (max_block_lines == 0) {
        dief(TMPLR_ERR_USAGE, "error: block length must be greater than 0\n");
    }
    save_block = calloc(max_block_lines, sizeof(*save_block));
    if (save_block == NULL) {
        dief(TMPLR_ERR_OOM, "error: could not allocate block buffer\n");
    }
}

const char *
get_pair(pair_t map[MAX_KEYS], const char *key)
{
    for (int i = 0; i < MAX_KEYS && strlen(map[i].key) != 0; i++)
        if (strcmp(map[i].key, key) == 0)
            return map[i].val;
    return NULL;
}


/* takes to strings of elements separated by sep and outputs in dst only the
 * elements that are contained in both strings. The dst string be again of the
 * form element<sep>element... */
void
intersect(char *dst, char *other, const char *sep)
{
    if (dst == NULL || other == NULL || sep == NULL)
        return;


    /* dinamically allocate a buffer to support variable length */
    char *dst_buf   = calloc(max_vlen + 1, sizeof(char));
    char *other_buf = calloc(max_vlen + 1, sizeof(char));

    char **allowed = calloc(max_vlen + 1, sizeof(char *));

    if (!dst_buf || !other_buf || !allowed) {
        dief(TMPLR_ERR_OOM, "error: out of memory in intersect\n");
    }

    strncpy(dst_buf, dst, sizeof(dst_buf) - 1);
    strncpy(other_buf, other, sizeof(other_buf) - 1);

    size_t allowed_cnt = 0;
    char *saveptr      = NULL;
    char *tok          = strtok_r(dst_buf, sep, &saveptr);

    while (tok) {
        trims(tok, " []");
        if (tok[0] != '\0')
            allowed[allowed_cnt++] = tok;
        tok = strtok_r(NULL, sep, &saveptr);
    }

    dst[0]            = '\0';
    bool first_token  = true;
    const size_t slen = strlen(sep);

    saveptr = NULL;
    tok     = strtok_r(other_buf, sep, &saveptr);
    while (tok) {
        trims(tok, " []");
        if (tok[0] != '\0') {
            for (size_t i = 0; i < allowed_cnt; i++) {
                if (strcmp(tok, allowed[i]) != 0)
                    continue;

                size_t dst_len = strlen(dst);
                size_t tok_len = strlen(tok);
                if (!first_token) {
                    if (dst_len + slen >= max_vlen)
                        goto overflow;
                    strcat(dst, sep);
                    dst_len += slen;
                }
                if (dst_len + tok_len >= max_vlen)
                    goto overflow;
                strcat(dst, tok);
                first_token = false;
                break;
            }
        }
        tok = strtok_r(NULL, sep, &saveptr);
    }
    free(dst_buf);
    free(other_buf);
    free(allowed);
    return;

overflow:
    dief(TMPLR_ERR_LIMIT,
         "error: filter intersection exceeds value length (%zu)\n", max_vlen);
    free(dst_buf);
    free(other_buf);
    free(allowed);
    return;
}

/* process_block is a recursive function that repeats the block for every
 * combination of values in the iteratin map.
 *
 * The parameter i indicates which variable out of nvars that has we have to
 * select a value. Once we have selected values for all variables, i == nvars,
 * and one block iteration is processed. The parameters count and last indicate
 * the current iteration count and whether the current iteration is the last one
 * of this template block. */
void
process_block(int i, const int nvars, int *count, bool last)
{
    pair_t *hook = NULL;

    if (i == nvars) {
        char icount[V_MIN_LEN];
        snprintf(icount, sizeof(icount), "%d", *count);

        remap(iteration_map, TMPL_ICOUNT, icount);
        remap(iteration_map, TMPL_ISLAST, last ? "true" : "false");

        if ((hook = find(block_hooks, "begin")))
            if (!process_block_line(hook->val))
                goto end;

        for (size_t k = 0; k < save_k && process_block_line(save_block[k]); k++)
            ;

        if ((hook = find(block_hooks, "end")))
            (void)process_block_line(hook->val);

end:
        (*count)++;
        unmap(iteration_map, TMPL_ICOUNT);
        unmap(iteration_map, TMPL_ISLAST);

        return;
    }

    pair_t *p = template_map + i;

    char *val = calloc(max_vlen + 1, sizeof(char));
    if (!val) {
        dief(TMPLR_ERR_OOM, "error: out of memory\n");
    }

    strncat(val, p->val, max_vlen);

    const char *sep = OPTION(ITER_SEP);
    char *saveptr   = NULL;

    char *tok         = NULL;
    const char *oval_ = get_pair(override_map, p->key);
    const char *fval_ = get_pair(filter_map, p->key);
    char *oval        = oval_ ? strdup(oval_) : NULL;
    char *fval        = fval_ ? strdup(fval_) : NULL;

    if (oval != NULL) {
        /* if there are defined values (with -DVAR=VAL1;VAL2),
        discard all other values of tok and only use those. */
        tok = strtok_r(oval, sep, &saveptr);
    } else if (fval != NULL) {
        intersect(fval, val, sep);
        tok = strtok_r(fval, sep, &saveptr);
    } else {
        tok = strtok_r(val, sep, &saveptr);
    }

    while (tok) {
        trims(tok, " ");
        remap(iteration_map, p->key, tok);
        tok = strtok_r(0, sep, &saveptr);
        process_block(i + 1, nvars, count, last && (!tok));
        unmap(iteration_map, p->key);
    }

    if (oval != NULL)
        free(oval);
    if (fval != NULL)
        free(fval);

    free(val);

    if (i == 0 && (hook = find(block_hooks, "final"))) {
        (void)process_block_line(hook->val);
    }
}

/*******************************************************************************
 * File processing
 ******************************************************************************/

/* processing state */
enum state {
    TEXT,
    IGNORE_BLOCK,
    BLOCK_BEGIN,
    BLOCK_BEGIN_ARGS,
    BLOCK_TEXT,
    BLOCK_END,

    MAP,
    HOOK,
} S = TEXT;


err_t
process_line(char *line)
{
    char *cur = NULL;
    char *end = NULL;
    err_t err;

again:
    switch (S) {
        case TEXT:
            if (!muted && strstr(line, TMPL_ABORT)) {
                dief(TMPLR_ERR_USAGE, "error: aborted by template command\n");
            }
            if (!muted && strstr(line, TMPL_BEGIN)) {
                S = BLOCK_BEGIN;
                goto again;
            }
            if (!muted && strstr(line, TMPL_MAP)) {
                S = MAP;
                goto again;
            }
            if (!muted && strstr(line, TMPL_HOOK)) {
                S = HOOK;
                goto again;
            }
            if (!muted && strstr(line, TMPL_MUTE)) {
                debugf("[OUTPUT] muted\n");
                muted = true;
                break;
            }
            if (strstr(line, TMPL_UNMUTE)) {
                debugf("[OUTPUT] unmuted\n");
                muted = false;
                break;
            }
            if (!muted && strstr(line, TMPL_DL) == NULL)
                emit_buf(line, strlen(line));
            break;

        case BLOCK_BEGIN:
            clean(template_map);
            cur = strstr(line, "(");
            if (cur == NULL)
                return ERROR("expected '('");
            S    = BLOCK_BEGIN_ARGS;
            line = cur;
            goto again;

        case BLOCK_BEGIN_ARGS:
            if ((end = strstr(line, ")"))) {
                err = parse_template_map(line, end);
                if (IS_ERROR(err))
                    return err;
                S = BLOCK_TEXT;
                process_begin();
            } else {
                if ((end = strstr(line, ",")) == NULL)
                    return ERROR("expected ','");
                for (char *e = NULL; (e = strstr(end + 1, ","), e && e != end);
                     end     = e)
                    ;
                parse_template_map(line, end);
            }
            break;

        case BLOCK_TEXT:
            cur = strstr(line, TMPL_END);
            if (cur != NULL) {
                S = BLOCK_END;
                goto again;
            }
            if (save_k >= max_block_lines)
                return ERROR("too many lines in block");

            memcpy(save_block[save_k++], line, strlen(line) + 1);
            break;
        case BLOCK_END:
            /* consume */
            {
                int nvars = 0;
                for (int i = 0; i < MAX_KEYS; i++)
                    if (template_map[i].key[0])
                        nvars++;
                int count = 0;
                process_block(0, nvars, &count, true);
            }
            save_k = 0;
            S      = TEXT;
            break;

        case MAP:
            if ((cur = strstr(line, "(")) == NULL)
                return ERROR("expected '('");
            if ((end = strstr(cur, ")")) == NULL)
                return ERROR("expected ')'");
            for (char *e = NULL; (e = strstr(end + 1, ")"), e && e != end);
                 end     = e)
                ;
            err = parse_assign(persistent_map, cur, end);
            if (IS_ERROR(err))
                return err;
            S = TEXT;
            break;

        case HOOK:
            if ((cur = strstr(line, "(")) == NULL)
                return ERROR("expected '('");
            if ((end = strstr(cur, ")")) == NULL)
                return ERROR("expected ')'");
            err = parse_assign(block_hooks, cur, end);
            if (IS_ERROR(err))
                return err;
            S = TEXT;
            break;

        default:
            assert(0 && "invalid");
    }
    return NO_ERROR;
}

/*******************************************************************************
 * File processing
 ******************************************************************************/

void
process_fp(FILE *fp, const char *fn)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    err_t err;
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        assert(line);
        err = process_line(line);
        if (IS_ERROR(err)) {
            dief(TMPLR_ERR_PARSE, "%s:%d: error: %s\n", fn, i + 1, err.msg);
        }
        if (line) {
            free(line);
            line = NULL;
        }
        i++;
    }
    if (line)
        free(line);
}
void
process_file(const char *fn)
{
    FILE *fp = fopen(fn, "r+");
    if (!fp) {
        dief(TMPLR_ERR_IO, "%s: %s\n", fn, strerror(errno));
    }
    process_fp(fp, fn);
    fclose(fp);
}

/*******************************************************************************
 * Library API
 ******************************************************************************/

static void
free_entries(map_entry_t *entries, size_t n)
{
    if (entries == NULL)
        return;
    for (size_t i = 0; i < n; i++) {
        free(entries[i].key);
        free(entries[i].val);
    }
    free(entries);
}

static void
clear_commands(void)
{
    char **cmds[] = {&TMPL_MAP,    &TMPL_BEGIN,  &TMPL_END,    &TMPL_MUTE,
                     &TMPL_UNMUTE, &TMPL_ABORT,  &TMPL_SKIP,   &TMPL_KILL,
                     &TMPL_UNDO,   &TMPL_DL,     &TMPL_NL,     &TMPL_UPCASE,
                     &TMPL_HOOK,   &TMPL_ICOUNT, &TMPL_ISLAST, NULL};
    for (int i = 0; cmds[i] != NULL; i++) {
        free(*cmds[i]);
        *cmds[i] = NULL;
    }
}

static tmplr_err
map_set(map_entry_t **entries, size_t *len, size_t *cap, const char *key,
        const char *val)
{
    if (key == NULL || val == NULL)
        return TMPLR_ERR_USAGE;

    for (size_t i = 0; i < *len; i++) {
        if (strcmp((*entries)[i].key, key) != 0)
            continue;
        char *new_val = strdup(val);
        if (new_val == NULL)
            return TMPLR_ERR_OOM;
        free((*entries)[i].val);
        (*entries)[i].val = new_val;
        return TMPLR_OK;
    }

    if (*len == *cap) {
        size_t new_cap      = (*cap == 0) ? 8 : (*cap * 2);
        map_entry_t *newptr = realloc(*entries, new_cap * sizeof(map_entry_t));
        if (newptr == NULL)
            return TMPLR_ERR_OOM;
        *entries = newptr;
        *cap     = new_cap;
    }
    (*entries)[*len].key = strdup(key);
    (*entries)[*len].val = strdup(val);
    if ((*entries)[*len].key == NULL || (*entries)[*len].val == NULL) {
        free((*entries)[*len].key);
        free((*entries)[*len].val);
        (*entries)[*len].key = NULL;
        (*entries)[*len].val = NULL;
        return TMPLR_ERR_OOM;
    }
    (*len)++;
    return TMPLR_OK;
}

static void
clear_runtime_state(void)
{
    clean(template_map);
    clean(override_map);
    clean(filter_map);
    clean(iteration_map);
    clean(persistent_map);
    clean(block_hooks);

    if (save_block != NULL) {
        free(save_block);
        save_block = NULL;
    }
    save_k = 0;
    muted  = false;
    S      = TEXT;
    clear_commands();
}

static void
prepare_runtime(const tmplr_ctx *ctx)
{
    clear_runtime_state();

    _verbose        = ctx->opts.verbose != 0;
    max_block_lines = ctx->opts.max_block_lines;
    max_vlen        = ctx->opts.max_value_len;

    if (max_block_lines == 0)
        dief(TMPLR_ERR_USAGE, "error: block length must be greater than 0\n");
    if (max_vlen < V_MIN_LEN) {
        dief(TMPLR_ERR_USAGE, "error: maximum length must be at least %u\n",
             (unsigned)V_MIN_LEN);
    }
    if (max_vlen > ABSOLUTE_MAX_VLEN) {
        dief(TMPLR_ERR_USAGE, "error: maximum length must be at max %lu\n",
             (size_t)ABSOLUTE_MAX_VLEN);
    }

    options[OPT_KV_SEP].val   = ",";
    options[OPT_ITEM_SEP].val = (char *)ctx->opts.item_sep;
    options[OPT_ITER_SEP].val = (char *)ctx->opts.iter_sep;

    allocate_block_buffer();
    set_prefix(ctx->opts.prefix);

    for (size_t i = 0; i < ctx->override_len; i++) {
        remap(override_map, ctx->override_entries[i].key,
              ctx->override_entries[i].val);
    }
    for (size_t i = 0; i < ctx->filter_len; i++) {
        remap(filter_map, ctx->filter_entries[i].key,
              ctx->filter_entries[i].val);
    }
}

static void
cleanup_runtime(void)
{
    clear_runtime_state();
}

tmplr_ctx *
tmplr_create(const tmplr_opts *opts)
{
    tmplr_ctx *ctx = calloc(1, sizeof(*ctx));
    if (ctx == NULL)
        return NULL;

    ctx->opts.max_block_lines = (opts && opts->max_block_lines) ?
                                    opts->max_block_lines :
                                    (size_t)MAX_BLEN;
    ctx->opts.max_value_len =
        (opts && opts->max_value_len) ? opts->max_value_len : (size_t)256;
    ctx->opts.verbose  = opts ? opts->verbose : 0;
    ctx->opts.prefix   = NULL;
    ctx->opts.item_sep = NULL;
    ctx->opts.iter_sep = NULL;

    if (ctx->opts.max_block_lines == 0 || ctx->opts.max_value_len < V_MIN_LEN ||
        ctx->opts.max_value_len > ABSOLUTE_MAX_VLEN) {
        free(ctx);
        return NULL;
    }

    const char *prefix   = (opts ? opts->prefix : NULL);
    const char *item_sep = (opts && opts->item_sep) ? opts->item_sep : ",";
    const char *iter_sep = (opts && opts->iter_sep) ? opts->iter_sep : ";";

    if (prefix != NULL) {
        char *copy = strdup(prefix);
        if (copy == NULL) {
            free(ctx);
            return NULL;
        }
        ctx->opts.prefix = copy;
    }
    ctx->opts.item_sep = strdup(item_sep);
    ctx->opts.iter_sep = strdup(iter_sep);
    if (ctx->opts.item_sep == NULL || ctx->opts.iter_sep == NULL) {
        free((void *)ctx->opts.prefix);
        free((void *)ctx->opts.item_sep);
        free((void *)ctx->opts.iter_sep);
        free(ctx);
        return NULL;
    }

    return ctx;
}

void
tmplr_destroy(tmplr_ctx *ctx)
{
    if (ctx == NULL)
        return;
    free_entries(ctx->override_entries, ctx->override_len);
    free_entries(ctx->filter_entries, ctx->filter_len);
    free((void *)ctx->opts.prefix);
    free((void *)ctx->opts.item_sep);
    free((void *)ctx->opts.iter_sep);
    free(ctx);
}

void
tmplr_reset(tmplr_ctx *ctx)
{
    if (ctx == NULL)
        return;
    free_entries(ctx->override_entries, ctx->override_len);
    free_entries(ctx->filter_entries, ctx->filter_len);
    ctx->override_entries = NULL;
    ctx->filter_entries   = NULL;
    ctx->override_len     = 0;
    ctx->filter_len       = 0;
    ctx->override_cap     = 0;
    ctx->filter_cap       = 0;
}

tmplr_err
tmplr_set_override(tmplr_ctx *ctx, const char *key, const char *values)
{
    if (ctx == NULL)
        return TMPLR_ERR_USAGE;
    return map_set(&ctx->override_entries, &ctx->override_len,
                   &ctx->override_cap, key, values);
}

tmplr_err
tmplr_set_filter(tmplr_ctx *ctx, const char *key, const char *values)
{
    if (ctx == NULL)
        return TMPLR_ERR_USAGE;
    return map_set(&ctx->filter_entries, &ctx->filter_len, &ctx->filter_cap,
                   key, values);
}

tmplr_err
tmplr_process_fp(tmplr_ctx *ctx, FILE *in, const char *name, tmplr_sink_fn sink,
                 void *sink_user)
{
    if (ctx == NULL || in == NULL)
        return TMPLR_ERR_USAGE;

    jmp_buf env;
    jmp_buf *prev_env       = trap_env;
    tmplr_err prev_err      = trap_err;
    tmplr_sink_fn prev_sink = active_sink;
    void *prev_sink_user    = active_sink_user;

    trap_env         = &env;
    trap_err         = TMPLR_OK;
    active_sink      = sink;
    active_sink_user = sink_user;

    if (setjmp(env) == 0) {
        prepare_runtime(ctx);
        process_fp(in, (name != NULL) ? name : "<input>");
    }
    cleanup_runtime();

    tmplr_err err    = trap_err;
    trap_env         = prev_env;
    trap_err         = prev_err;
    active_sink      = prev_sink;
    active_sink_user = prev_sink_user;
    return err;
}

tmplr_err
tmplr_process_file(tmplr_ctx *ctx, const char *path, tmplr_sink_fn sink,
                   void *sink_user)
{
    if (ctx == NULL || path == NULL)
        return TMPLR_ERR_USAGE;

    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return TMPLR_ERR_IO;

    tmplr_err err = tmplr_process_fp(ctx, fp, path, sink, sink_user);
    fclose(fp);
    return err;
}

tmplr_err
tmplr_process_string(tmplr_ctx *ctx, const char *input, size_t len,
                     tmplr_sink_fn sink, void *sink_user)
{
    if (ctx == NULL || input == NULL)
        return TMPLR_ERR_USAGE;
    if (len == 0)
        len = strlen(input);

    FILE *fp = tmpfile();
    if (fp == NULL)
        return TMPLR_ERR_IO;
    if (fwrite(input, 1, len, fp) != len) {
        fclose(fp);
        return TMPLR_ERR_IO;
    }
    rewind(fp);
    tmplr_err err = tmplr_process_fp(ctx, fp, "<string>", sink, sink_user);
    fclose(fp);
    return err;
}

const char *
tmplr_strerror(tmplr_err err)
{
    switch (err) {
        case TMPLR_OK:
            return "ok";
        case TMPLR_ERR_PARSE:
            return "parse error";
        case TMPLR_ERR_IO:
            return "io error";
        case TMPLR_ERR_OOM:
            return "out of memory";
        case TMPLR_ERR_LIMIT:
            return "limit exceeded";
        case TMPLR_ERR_USAGE:
            return "invalid usage";
        default:
            return "unknown error";
    }
}

#ifndef TMPLR_NO_MAIN
/*******************************************************************************
 * main function with options
 *
 * $0 [-v] <FILE> [FILE]...
 ******************************************************************************/
int
main(int argc, char *argv[])
{
    bool read_stdin = false;
    char *prefix    = NULL;
    debugf("vatomic generator\n");
    int c;
    char *k;
    while ((c = getopt(argc, argv, "b:l:hisvVP:D:F:")) != -1) {
        switch (c) {
            case 'b': {
                char *endptr      = NULL;
                errno             = 0;
                unsigned long val = strtoul(optarg, &endptr, 10);
                if (errno != 0 || endptr == optarg || *endptr != '\0' ||
                    val == 0) {
                    fprintf(stderr, "error: invalid block length '%s'\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
                max_block_lines = val;
                break;
            }
            case 'D':
                k    = strstr(optarg, "=");
                *k++ = '\0';
                remap(override_map, optarg, k);
                break;
            case 'F':
                k    = strstr(optarg, "=");
                *k++ = '\0';
                remap(filter_map, optarg, k);
                break;
            case 'l': {
                char *endptr      = NULL;
                errno             = 0;
                unsigned long val = strtoul(optarg, &endptr, 10);
                if (errno != 0 || endptr == optarg || *endptr != '\0' ||
                    val == 0) {
                    fprintf(stderr, "error: invalid maximum length '%s'\n",
                            optarg);
                    exit(EXIT_FAILURE);
                }
                if (val > ABSOLUTE_MAX_VLEN) {
                    fprintf(stderr,
                            "error: maximum length must be at max %lu\n",
                            ABSOLUTE_MAX_VLEN);
                    exit(EXIT_FAILURE);
                }
                if (val < V_MIN_LEN) {
                    fprintf(stderr,
                            "error: maximum length must be at least %u\n",
                            (unsigned)V_MIN_LEN);
                    exit(EXIT_FAILURE);
                }
                max_vlen = (size_t)val;
                break;
            }
            case 'P':
                prefix = strdup(optarg);
                break;
            case 'V':
                printf("%s\n", TMPLR_VERSION);
                exit(0);
            case 'v':
                _verbose = true;
                break;
            case 'i':
                read_stdin = true;
                break;
            case 's':
                /* By default, item and iter separators are ',' and ';',
                 * respectively. This option swaps them, so their use is:
                 * Consider the following example:
                 *     _begin(X = [[A;B]], Y = [[C;D]]).
                 * Now, when pass -s option, the user would write:
                 *     _begin(X = [[A;B]], Y = [[C;D]]).
                 */
                set_option(OPT_ITEM_SEP, ";");
                set_option(OPT_ITER_SEP, ",");
                break;
            case 'h':
                printf("tmplr v%s - a simple templating tool\n\n",
                       TMPLR_VERSION);
                printf("Usage:\n\ttmplr [FLAGS] <FILE> [FILE ...]\n\n");
                printf("Flags:\n");
                printf("\t-Dkey=value   override template map assignement\n");
                printf("\t-Fkey=value   filter template map assignement\n");
                printf(
                    "\t-b LINES      set maximum lines buffered per block "
                    "(default %zu)\n",
                    (size_t)MAX_BLEN);
                printf(
                    "\t-l CHARS      set maximum length of a single template "
                    "value "
                    "(default %zu)\n",
                    (size_t)max_vlen);
                printf("\t-i            read stdin\n");
                printf("\t-P PREFIX     use PREFIX instead of $ prefix\n");
                printf("\t-s            swap iterator and item separators\n");
                printf("\t-v            verbose\n");
                printf("\t-V            show version\n");
                exit(0);
            case '?':
                printf("error");
                exit(1);
            default:
                break;
        }
    }
    allocate_block_buffer();
    set_prefix(prefix);

    for (int i = optind; i < argc; i++)
        process_file(argv[i]);
    if (read_stdin)
        process_fp(stdin, "<stdin>");

    free(prefix);
    return 0;
}
#endif
