/*
 * Copyright (C) 2025-2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 *
 * tmplr macros
 *
 * These macros expand to nothing so that tmplr commands can live inside C or
 * C++ sources without confusing the compiler or LSP tooling. See tmplr(1) for
 * the runtime behaviour; the notes below summarize the user-facing API.
 */
#ifndef TMPLR_MACROS_H
#define TMPLR_MACROS_H

/**
 * Marks the beginning of a template block.
 *
 * Accepts a comma-separated list of key-value pairs, where each value may be a
 * single literal or a list in the form [[val1;val2;val3]]. tmplr iterates over
 * the cartesian product of the provided values and emits the block once per
 * combination. Example:
 *
 * ```c
 * $_begin(KEY1 = VALUE1, KEY2 = [[VALUE2; VALUE3]]);
 * KEY1 = KEY2;
 * $_end;
 * ```
 */
#define $_begin(...)

/**
 * Marks the end of a template block.
 */
#define $_end

/**
 * Registers content for a block hook.
 *
 * @param HOOK One of begin, end, or final.
 *
 * Hook values are treated as synthetic lines that go through the same mapping
 * logic as the rest of the block. begin/end hooks run for every iteration,
 * whereas final runs once after the block is completely processed.
 */
#define $_hook(HOOK, ...)

/**
 * Stops tmplr processing and output.
 *
 * Until the matching $_unmute, all text is discarded and all tmplr command
 * ignored. A muted-block is useful to add includes that help LSP servers.
 */
#define $_mute

/**
 * Restarts tmplr processing output.
 */
#define $_unmute

/**
 * Adds or overrides a persistent mapping that applies outside of blocks.
 *
 * Can only be used when tmplr is not in the middle of a template block.
 */
#define $_map(K, ...)

/**
 * Skips template block iteration.
 *
 * When invoked inside a block, the current iteration stops immediately and the
 * remaining lines of the block are discarded.
 */
#define $_skip

/**
 * Truncates the rest of the current line at the point of invocation.
 */
#define $_kill

/**
 * Removes everything emitted on the current line prior to the command.
 */
#define $_undo

/**
 * Deletes the rest of the current line from the template output.
 */
#define $_dl

/**
 * Inserts an explicit newline in the output.
 */
#define $_nl

/**
 * Aborts tmplr execution and exits with error code 1.
 */
#define $_abort

/**
 * Makes content uppercase.
 *
 * Accepts either parentheses or a custom delimiter, e.g. $_upcase(KEY) or
 * $_upcase%literal%. Useful inside template blocks.
 */
#define $_upcase(...)

#endif /* TMPLR_MACROS_H */
