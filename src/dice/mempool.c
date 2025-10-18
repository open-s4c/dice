/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dice/interpose.h>
#include <dice/mempool.h>
#include <vsync/spinlock/caslock.h>

static size_t _sizes[] = {32,
                          128,
                          512,
                          1024,
                          2048,
                          8192,
                          1 * 1024 * 1024,
                          4 * 1024 * 1024,
                          8 * 1024 * 1024};
#define NSTACKS (sizeof(_sizes) / sizeof(size_t))

#define MEMPOOL_SIZE (1024 * 1024 * 200)

#ifdef ALLOC_STRICT_ALIGN8
#define ALIGNMENT 8
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#endif

static unsigned int
_bucketize(size_t size)
{
    unsigned int i = 0;
    for (; i < NSTACKS && size > _sizes[i]; i++)
        ;
    assert(i < NSTACKS);
    return i;
}

typedef struct entry {
    struct entry *next;
    size_t size;
    char data[];
} entry_t;

typedef struct alloc {
    size_t capacity;
    size_t next;
    char *memory;
} alloc_t;

typedef struct mempool {
    caslock_t lock;
    size_t allocated;
    struct alloc pool;
    entry_t *stack[NSTACKS];
} mempool_t;

static mempool_t _mp;

/* bypass malloc interceptor */
REAL_DECL(void *, malloc, size_t n);

DICE_HIDE void
mempool_init(size_t cap)
{
    memset(&_mp.stack, 0, sizeof(entry_t *) * NSTACKS);
    _mp.allocated     = 0;
    _mp.pool.capacity = cap;
    _mp.pool.next     = 0;
    _mp.pool.memory   = REAL_CALL(malloc, 0, cap);
    assert(_mp.pool.memory);
    memset(_mp.pool.memory, 0, cap);

    #ifdef ALLOC_STRICT_ALIGN8
    _mp.pool.next = ALIGN_UP((size_t)_mp.pool.memory, ALIGNMENT) - (size_t)_mp.pool.memory;
    #endif
    // caslock already initialized with 0
}

static inline void
mempool_ensure_initd(void)
{
    // assumes protected by lock
    if (_mp.pool.memory == NULL)
        mempool_init(MEMPOOL_SIZE);
}

DICE_MODULE_INIT({
    caslock_acquire(&_mp.lock);
    mempool_ensure_initd();
    caslock_release(&_mp.lock);
})

DICE_HIDE_IF void *
mempool_alloc(size_t n)
{
    mempool_t *mp   = &_mp;
    entry_t *e      = NULL;
    size_t size     = n + sizeof(entry_t);
    unsigned bucket = _bucketize(size);
    size            = _sizes[bucket];
    #ifdef ALLOC_STRICT_ALIGN8
    size = ALIGN_UP(size, ALIGNMENT);
    #endif
    entry_t **stack = &mp->stack[bucket];
    assert(stack);

    // Mempool is used from rogue thread, serialization is necessary
    caslock_acquire(&mp->lock);

    if (*stack) {
        e       = *stack;
        *stack  = e->next;
        e->next = NULL;
        mp->allocated += size;
        goto out;
    }

    mempool_ensure_initd();

    if (e == NULL && mp->pool.capacity >= mp->pool.next + size) {
        e       = (entry_t *)(mp->pool.memory + mp->pool.next);
        e->next = NULL;
        e->size = n;
        mp->pool.next += size;
        mp->allocated += size;
    }
out:
    caslock_release(&mp->lock);
    return e ? e->data : NULL;
}

DICE_HIDE_IF void *
mempool_realloc(void *ptr, size_t size)
{
    void *p = mempool_alloc(size);
    if (!p || !ptr)
        return p;
    entry_t *e = (entry_t *)ptr - 1;
    size       = e->size < size ? e->size : size;
    memcpy(p, ptr, size);
    free(ptr);
    return p;
}

DICE_HIDE_IF void
mempool_free(void *ptr)
{
    mempool_t *mp = &_mp;
    assert(ptr);
    entry_t *e      = (entry_t *)ptr - 1;
    size_t size     = e->size + sizeof(entry_t);
    unsigned bucket = _bucketize(size);
    size            = _sizes[bucket];
    #ifdef ALLOC_STRICT_ALIGN8
    size = ALIGN_UP(size, ALIGNMENT);
    #endif
    entry_t **stack = &mp->stack[bucket];

    // Mempool is used from rogue thread, serialization is necessary
    caslock_acquire(&mp->lock);
    mp->allocated -= size;
    assert(stack);
    e->next = *stack;
    *stack  = e;
    caslock_release(&mp->lock);
}
