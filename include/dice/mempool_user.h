/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 * -----------------------------------------------------------------------------
 * @file mempool_user.h
 * @brief Pool allocator safe during load time.
 *
 * It is recommended that all allocations inside Dice modules use the memory
 * pool component.  On initialization of libdice, a memory pool is created by
 * the main thread.  New threads can safely allocate from the memory pool even
 * before TLS region is properly initialized.
 */
#ifndef DICE_MEMPOOL_USER_H
#define DICE_MEMPOOL_USER_H
#include <stddef.h>

/* mempool_user_alloc allocates a region of memory of `size` bytes.
 *
 * Returns NULL if out of memory.
 */
void *mempool_user_alloc(size_t size);

/* mempool_user_realloc reallocates a region of memory with `size` bytes.
 *
 * Accessing `ptr` after mempool_user_realloc is undefined behavior.
 * Returns NULL if out of memory.
 */
void *mempool_user_realloc(void *ptr, size_t size);

/* mempool_user_free frees a region of memory previously allocated with mempool_user.
 *
 * If ptr is NULL the function does nothing. The behavior is undefined if ptr
 * points to a region not allocated with mempool_user.
 */
void mempool_user_free(void *ptr);

/* mempool_user_aligned_alloc allocates a region of memory of `size` bytes aligned by
 * alignment, which must be a power of 2.
 *
 * Returns a pointer to the allocated memory which is multiple of alignment or
 * NULL if out of memory.
 *
 */
void *mempool_user_aligned_alloc(size_t alignment, size_t size);

size_t mempool_user_usable_size(void * ptr);

#endif /* DICE_MEMPOOL_USER_H */
