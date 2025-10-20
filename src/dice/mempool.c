/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
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

static size_t sizes_[] = {32,
                          128,
                          512,
                          1024,
                          2048,
                          8192,
                          1 * 1024 * 1024,
                          4 * 1024 * 1024,
                          8 * 1024 * 1024};
#define NSTACKS (sizeof(sizes_) / sizeof(size_t))

#define MEMPOOL_SIZE (1024 * 1024 * 200)

static unsigned int
bucketize_(size_t size)
{
    unsigned int i = 0;
    for (; i < NSTACKS && size > sizes_[i]; i++)
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

static mempool_t mp_;

/* bypass malloc interceptor */
REAL_DECL(void *, malloc, size_t n);

DICE_HIDE void
mempool_init(size_t cap)
{
    memset(&mp_.stack, 0, sizeof(entry_t *) * NSTACKS);
    mp_.allocated     = 0;
    mp_.pool.capacity = cap;
    mp_.pool.next     = 0;
    mp_.pool.memory   = REAL_CALL(malloc, 0, cap);
    assert(mp_.pool.memory);
    memset(mp_.pool.memory, 0, cap);
    // caslock already initialized with 0
}

static inline void
mempool_ensure_initd(void)
{
    // assumes protected by lock
    if (mp_.pool.memory == NULL)
        mempool_init(MEMPOOL_SIZE);
}

DICE_MODULE_INIT({
    caslock_acquire(&mp_.lock);
    mempool_ensure_initd();
    caslock_release(&mp_.lock);
})

DICE_HIDE_IF void *
mempool_alloc(size_t n)
{
    mempool_t *mp   = &mp_;
    entry_t *e      = NULL;
    size_t size     = n + sizeof(entry_t);
    unsigned bucket = bucketize_(size);
    size            = sizes_[bucket];
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
    mempool_t *mp = &mp_;
    assert(ptr);
    entry_t *e      = (entry_t *)ptr - 1;
    size_t size     = e->size + sizeof(entry_t);
    unsigned bucket = bucketize_(size);
    size            = sizes_[bucket];
    entry_t **stack = &mp->stack[bucket];

    // Mempool is used from rogue thread, serialization is necessary
    caslock_acquire(&mp->lock);
    mp->allocated -= size;
    assert(stack);
    e->next = *stack;
    *stack  = e;
    caslock_release(&mp->lock);
}
