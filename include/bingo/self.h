/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file self.h
 * @brief Self-awareness module
 *
 * Bingo's self module is part of the core library. It provides safely
 * allocated TLS and unique thread identifiers.
 */
#ifndef BINGO_SELF_H
#define BINGO_SELF_H
#include <stddef.h>

#include <bingo/pubsub.h>
#include <bingo/thread_id.h>

/* Get unique thread id */
thread_id self_id(metadata_t *self);

/* Get or allocate a memory area in TLS.
 *
 * `global` must be a unique pointer, typically a global variable of the desired
 * type.
 */
void *self_tls(metadata_t *self, const void *global, size_t size);


/* Helper macro that gets or creates a memory area with the size of the type
 * pointed by global_ptr.
 */
#define SELF_TLS(self, global_ptr)                                             \
    ((__typeof(global_ptr))self_tls((self), (global_ptr),                      \
                                    sizeof(*(global_ptr))))


/* Standard capture chains. */
#define CAPTURE_EVENT  4
#define CAPTURE_BEFORE 5
#define CAPTURE_AFTER  6

#endif /* BINGO_SELF_H */
