/*
 * Copyright (C) 2025-2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 *
 * tmplr library API header
 *
 * This file defines the public API for the tmplr library, which provides
 * template processing functionality.
 */
#ifndef TMPLR_H
#define TMPLR_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque context structure for tmplr operations
 *
 */
typedef struct tmplr_ctx tmplr_ctx;

/**
 * Error codes returned by tmplr functions
 *
 */
typedef enum {
    TMPLR_OK = 0,    /**< Success */
    TMPLR_ERR_PARSE, /**< Parse error */
    TMPLR_ERR_IO,    /**< IO error */
    TMPLR_ERR_OOM,   /**< Out of memory */
    TMPLR_ERR_LIMIT, /**< Limit exceeded */
    TMPLR_ERR_USAGE  /**< Usage error */
} tmplr_err;

/**
 * Configuration options for tmplr
 *
 */
typedef struct {
    size_t max_block_lines; /**< Maximum number of lines in a block */
    size_t max_value_len;   /**< Maximum length of values */
    const char *prefix;     /**< Prefix for template variables */
    const char *item_sep;   /**< Separator for list items */
    const char *iter_sep;   /**< Separator for iteration */
    int verbose;            /**< Verbose output flag */
} tmplr_opts;

/**
 * Function pointer type for output sink
 *
 * @param buf Buffer containing output data
 * @param len Length of data in buffer
 * @param user User-defined pointer
 * @return 0 on success, non-zero on error
 */
typedef int (*tmplr_sink_fn)(const char *buf, size_t len, void *user);

/**
 * Create a new tmplr context
 *
 * @param opts Configuration options, or NULL for defaults
 * @return Pointer to new context, or NULL on error
 */
tmplr_ctx *tmplr_create(const tmplr_opts *opts);

/**
 * Destroy a tmplr context
 *
 * @param ctx Context to destroy
 */
void tmplr_destroy(tmplr_ctx *ctx);

/**
 * Reset a tmplr context
 *
 * @param ctx Context to reset
 */
void tmplr_reset(tmplr_ctx *ctx);

/**
 * Set override values for keys
 *
 * @param ctx Context to operate on
 * @param key Key to set overrides for
 * @param values Comma-separated list of override values
 * @return Error code
 */
tmplr_err tmplr_set_override(tmplr_ctx *ctx, const char *key,
                             const char *values);

/**
 * Set filter values for keys
 *
 * @param ctx Context to operate on
 * @param key Key to set filters for
 * @param values Comma-separated list of filter values
 * @return Error code
 */
tmplr_err tmplr_set_filter(tmplr_ctx *ctx, const char *key, const char *values);

/**
 * Process input from a file pointer
 *
 * @param ctx Context to operate on
 * @param in Input file pointer
 * @param name Name of input (for error reporting)
 * @param sink Output sink function
 * @param sink_user User pointer passed to sink function
 * @return Error code
 */
tmplr_err tmplr_process_fp(tmplr_ctx *ctx, FILE *in, const char *name,
                           tmplr_sink_fn sink, void *sink_user);

/**
 * Process input from a file
 *
 * @param ctx Context to operate on
 * @param path Path to input file
 * @param sink Output sink function
 * @param sink_user User pointer passed to sink function
 * @return Error code
 */
tmplr_err tmplr_process_file(tmplr_ctx *ctx, const char *path,
                             tmplr_sink_fn sink, void *sink_user);

/**
 * Process input from a string
 *
 * @param ctx Context to operate on
 * @param input Input string
 * @param len Length of input string
 * @param sink Output sink function
 * @param sink_user User pointer passed to sink function
 * @return Error code
 */
tmplr_err tmplr_process_string(tmplr_ctx *ctx, const char *input, size_t len,
                               tmplr_sink_fn sink, void *sink_user);

/**
 * Convert error code to string
 *
 * @param err Error code
 * @return String representation of error
 */
const char *tmplr_strerror(tmplr_err err);

#ifdef __cplusplus
}
#endif

#endif /* TMPLR_H */
