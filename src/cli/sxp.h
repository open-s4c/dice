#ifndef SXP_H
#define SXP_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sxp_kind { SXP_LIST, SXP_SYMBOL, SXP_STRING };
struct sxp {
    enum sxp_kind kind;
    unsigned line;
    char *text;
    struct sxp *child, *next;
};

struct sxp_error {
    unsigned line;
    char message[256];
};

/* Parse exactly one expression from the current position through EOF.
 * Supports lists, symbols, quoted strings and semicolon line comments.
 * String escapes: \n, \r, \t, \\ and \". No evaluation is performed.
 * Limits: node depth 128 (root is 0), 65536 nodes, 4095 bytes per token.
 * Returns an owned tree, or NULL on failure (including empty input).
 * On failure all partial allocations are freed. Optional error is cleared
 * on success and receives a one-based line and message on failure.
 * Does not close file, print diagnostics or terminate the process.
 * List text is NULL; symbol/string child is NULL. Text is NUL-terminated.
 */
struct sxp *sxp_read(FILE *file, struct sxp_error *error);

/* Free node, its children and its following siblings. NULL is accepted. */
void sxp_free(struct sxp *node);

#ifdef __cplusplus
}
#endif
#endif
