/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 * -----------------------------------------------------------------------------
 * @file self.h
 * @brief Self-awareness module
 *
 * Dice's self module is part of the core library. It provides safely
 * allocated TLS and unique thread identifiers.
 */
#ifndef DICE_SELF_H
#define DICE_SELF_H
#include <stddef.h>

#include <dice/chains/capture.h>
#include <dice/events/self.h>
#include <dice/pubsub.h>
#include <dice/thread_id.h>

/* Get unique thread id */
thread_id self_id(metadata_t *self);

bool self_retired(metadata_t *self);

/* Get or allocate a memory area in TLS.
 *
 * `global` must be a unique pointer, typically a global variable of the desired
 * type.
 */
void *self_tls(metadata_t *self, const void *global, size_t size);

/* Get pointer mapped to key if any.
 *
 * If no memory area is mapped to key, return NULL.
 */
void *self_tls_get(metadata_t *self, uintptr_t key);

struct tls_dtor {
    void (*free)(void *arg, void *ptr);
    void *arg;
};

/* Set key to map to pointer.
 *
 * If no memory area is mapped to key, return NULL.
 */
void self_tls_set(metadata_t *self, uintptr_t key, void *ptr,
                  struct tls_dtor dtor);


/* Helper macro that gets or creates a memory area with the size of the type
 * pointed by global_ptr.
 */
#define SELF_TLS(self, global_ptr)                                             \
    ((__typeof(global_ptr))self_tls((self), (global_ptr),                      \
                                    sizeof(*(global_ptr))))


#endif /* DICE_SELF_H */
