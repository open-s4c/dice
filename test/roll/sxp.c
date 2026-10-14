#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sxp.h"

#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);    \
            abort();                                                           \
        }                                                                      \
    } while (0)

static struct sxp *
parse(const char *text, struct sxp_error *error)
{
    FILE *file = tmpfile();
    CHECK(file);
    CHECK(fputs(text, file) >= 0);
    rewind(file);
    struct sxp *root = sxp_read(file, error);
    CHECK(fseek(file, 0, SEEK_SET) == 0);
    CHECK(fclose(file) == 0);
    return root;
}

static void
invalid(const char *text, unsigned line, const char *message)
{
    struct sxp_error error;
    CHECK(!parse(text, &error));
    CHECK(error.line == line);
    CHECK(strstr(error.message, message));
    CHECK(!parse(text, NULL));
}

static void
limits(void)
{
    char token[4097];
    memset(token, 'x', sizeof(token));
    token[4095]      = 0;
    struct sxp *root = parse(token, NULL);
    CHECK(root && strlen(root->text) == 4095);
    sxp_free(root);
    token[4095] = 'x';
    token[4096] = 0;
    invalid(token, 1, "token is too long");

    char nested[260];
    for (unsigned depth = 128; depth <= 129; depth++) {
        memset(nested, '(', depth);
        nested[depth] = 'x';
        memset(nested + depth + 1, ')', depth);
        nested[depth * 2 + 1] = 0;
        if (depth == 128) {
            root = parse(nested, NULL);
            CHECK(root);
            sxp_free(root);
        } else {
            invalid(nested, 1, "size/depth limit");
        }
    }

    FILE *file = tmpfile();
    CHECK(file);
    CHECK(fputc('(', file) != EOF);
    for (unsigned i = 0; i < 65535; i++)
        CHECK(fputs("x ", file) >= 0);
    long end = ftell(file);
    CHECK(end >= 0);
    CHECK(fputc(')', file) != EOF);
    rewind(file);
    root = sxp_read(file, NULL);
    CHECK(root);
    sxp_free(root);
    CHECK(fseek(file, end, SEEK_SET) == 0);
    CHECK(fputs("x)", file) >= 0);
    rewind(file);
    struct sxp_error error;
    CHECK(!sxp_read(file, &error));
    CHECK(strstr(error.message, "size/depth limit"));
    CHECK(fclose(file) == 0);
}

int
main(void)
{
    struct sxp_error error = {.line = 42, .message = "old error"};
    struct sxp *root       = parse(
        "; comment\n(root\n () \"a;()\\n\\r\\t\\\\\\\"\" "
              "symbol; comment\n) ; end",
        &error);
    CHECK(root && root->kind == SXP_LIST && root->line == 2);
    CHECK(!root->text && !root->next);
    CHECK(error.line == 0 && !error.message[0]);
    struct sxp *node = root->child;
    CHECK(node && node->kind == SXP_SYMBOL && !strcmp(node->text, "root"));
    CHECK(!node->child && node->line == 2);
    node = node->next;
    CHECK(node && node->kind == SXP_LIST && !node->child && node->line == 3);
    node = node->next;
    CHECK(node && node->kind == SXP_STRING);
    CHECK(!strcmp(node->text, "a;()\n\r\t\\\""));
    node = node->next;
    CHECK(node && !strcmp(node->text, "symbol") && !node->next);
    sxp_free(root);
    sxp_free(NULL);

    invalid("", 1, "expected an expression");
    invalid("; comment\n", 2, "expected an expression");
    invalid(")", 1, "expected an expression");
    invalid("(a\n (b)", 2, "unclosed list starting on line 1");
    invalid("(a) (b)", 1, "expected end of file");
    invalid("(\"abc", 1, "unterminated string");
    invalid("(\"\\q\")", 1, "unsupported string escape");
    invalid("'a", 1, "unexpected character");
    invalid("(a\n `b)", 2, "unexpected character");
    CHECK(!sxp_read(NULL, &error));
    CHECK(error.line == 1 && strstr(error.message, "input stream"));
    CHECK(!sxp_read(NULL, NULL));
    limits();

    /* A failed parse must not prevent later use in the same process. */
    root = parse("\"\"", &error);
    CHECK(root && root->kind == SXP_STRING && !root->text[0]);
    CHECK(!error.line && !error.message[0]);
    sxp_free(root);
    return 0;
}
