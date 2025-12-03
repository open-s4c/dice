/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 * -----------------------------------------------------------------------------
 * @file mempool.h
 * @brief Pool allocator safe during load time.
 *
 * It is recommended that all allocations inside Dice modules use the memory
 * pool component.  On initialization of libdice, a memory pool is created by
 * the main thread.  New threads can safely allocate from the memory pool even
 * before TLS region is properly initialized.
 */
#ifndef DICE_MEMPOOL_H
#define DICE_MEMPOOL_H
#include <stddef.h>

/* mempool_alloc allocates a region of memory of `size` bytes.
 *
 * Returns NULL if out of memory.
 */
void *mempool_alloc(size_t size);

/* mempool_realloc reallocates a region of memory with `size` bytes.
 *
 * Accessing `ptr` after mempool_realloc is undefined behavior.
 * Returns NULL if out of memory.
 */
void *mempool_realloc(void *ptr, size_t size);

/* mempool_free frees a region of memory previously allocated with mempool.
 *
 * If ptr is NULL the function does nothing. The behavior is undefined if ptr
 * points to a region not allocated with mempool.
 */
void mempool_free(void *ptr);

/* mempool_aligned_alloc allocates a region of memory of `size` bytes aligned by alignment, which must be a power of 2.
 *
 * Returns a pointer to the allocated memory which is multiple of alignment or NULL if out of memory.
 *
 */
void *mempool_aligned_alloc(size_t alignment, size_t size);

#endif /* DICE_MEMPOOL_H */
