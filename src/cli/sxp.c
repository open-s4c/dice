/* sxp version 0.1 */
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "sxp.h"

struct reader {
    FILE *file;
    struct sxp_error *error;
    unsigned line, nodes;
    int ch;
    int failed;
};

static void
error(struct reader *r, const char *fmt, ...)
{
    if (r->failed)
        return;
    r->failed = 1;
    if (!r->error)
        return;
    r->error->line = r->line;
    va_list args;
    va_start(args, fmt);
    vsnprintf(r->error->message, sizeof(r->error->message), fmt, args);
    va_end(args);
}

static void
advance(struct reader *r)
{
    if (r->failed)
        return;
    if (r->ch == '\n')
        r->line++;
    r->ch = fgetc(r->file);
    if (r->ch == EOF && ferror(r->file))
        error(r, "cannot read input");
}

static void
space(struct reader *r)
{
    while (!r->failed) {
        while (r->ch != EOF && isspace((unsigned char)r->ch))
            advance(r);
        if (r->ch != ';')
            return;
        while (r->ch != EOF && r->ch != '\n')
            advance(r);
    }
}

static struct sxp *
expression(struct reader *r, unsigned depth)
{
    space(r);
    if (r->failed)
        return NULL;
    if (depth > 128 || ++r->nodes > 65536) {
        error(r, "expression size/depth limit exceeded");
        return NULL;
    }
    struct sxp *n = calloc(1, sizeof(*n));
    if (!n) {
        error(r, "out of memory");
        return NULL;
    }
    n->line = r->line;
    if (r->ch == '(') {
        n->kind = SXP_LIST;
        advance(r);
        struct sxp **tail = &n->child;
        for (;;) {
            space(r);
            if (r->failed)
                goto fail;
            if (r->ch == EOF) {
                error(r, "unclosed list starting on line %u", n->line);
                goto fail;
            }
            if (r->ch == ')') {
                advance(r);
                if (r->failed)
                    goto fail;
                return n;
            }
            *tail = expression(r, depth + 1);
            if (!*tail)
                goto fail;
            tail = &(*tail)->next;
        }
    }
    if (r->ch == EOF || r->ch == ')') {
        error(r, "expected an expression");
        goto fail;
    }
    int quoted = r->ch == '"';
    n->kind    = quoted ? SXP_STRING : SXP_SYMBOL;
    if (quoted)
        advance(r);
    char text[4096];
    size_t len = 0;
    for (;;) {
        if (r->failed)
            goto fail;
        if (quoted) {
            if (r->ch == EOF) {
                error(r, "unterminated string");
                goto fail;
            }
            if (r->ch == '"') {
                advance(r);
                break;
            }
        } else if (r->ch == EOF || isspace((unsigned char)r->ch) ||
                   r->ch == '(' || r->ch == ')' || r->ch == ';') {
            break;
        }
        int ch = r->ch;
        if (quoted && ch == '\\') {
            advance(r);
            switch (r->ch) {
                case 'n':
                    ch = '\n';
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case '\\':
                case '"':
                    ch = r->ch;
                    break;
                default:
                    error(r, "unsupported string escape");
                    goto fail;
            }
        } else if (ch < 32 || ch == 127 ||
                   (!quoted &&
                    (ch == '"' || ch == '\'' || ch == '`' || ch == ','))) {
            error(r, "unexpected character");
            goto fail;
        }
        if (len + 1 == sizeof(text)) {
            error(r, "token is too long");
            goto fail;
        }
        text[len++] = (char)ch;
        advance(r);
    }
    text[len] = 0;
    if (r->failed)
        goto fail;
    n->text = malloc(len + 1);
    if (!n->text) {
        error(r, "out of memory");
        goto fail;
    }
    memcpy(n->text, text, len + 1);
    return n;
fail:
    sxp_free(n);
    return NULL;
}

struct sxp *
sxp_read(FILE *file, struct sxp_error *err)
{
    if (err)
        *err = (struct sxp_error){0};
    struct reader r = {.file = file, .error = err, .line = 1};
    if (!file) {
        error(&r, "input stream is NULL");
        return NULL;
    }
    advance(&r);
    struct sxp *root = expression(&r, 0);
    space(&r);
    if (r.ch != EOF)
        error(&r, "expected end of file after the root expression");
    if (r.failed) {
        sxp_free(root);
        return NULL;
    }
    return root;
}

void
sxp_free(struct sxp *n)
{
    while (n) {
        struct sxp *next = n->next;
        sxp_free(n->child);
        free(n->text);
        free(n);
        n = next;
    }
}
