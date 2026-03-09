/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dice/interpose.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <vsync/spinlock/caslock.h>

#ifdef DICE_MEMPOOL_USE_MMAP
#  include <sys/mman.h>
#endif

static size_t sizes_[] = {32,
                          128,
                          256,
                          512,
                          1 * 1024,
                          4 * 1024,
                          8 * 1024,
                          16 * 1024,
                          32 * 1024,
                          64 * 1024,
                          128 * 1024,
                          256 * 1024,
                          512 * 1024,
                          1 * 1024 * 1024,
                          4 * 1024 * 1024,
                          8 * 1024 * 1024,
                          16 * 1024 * 1024,
                          32 * 1024 * 1024,
                          64 * 1024 * 1024,
                          128 * 1024 * 1024,
                          256 * 1024 * 1024,
                          512 * 1024 * 1024,
                          1024 * 1024 * 1024,
                          (size_t)2 * 1024 * 1024 * 1024};
#define NSTACKS     (sizeof(sizes_) / sizeof(size_t))
#define HEADER_SIZE (sizeof(entry_t) + sizeof(entry_t *))

static unsigned int
bucketize_(size_t size)
{
    unsigned int i = 0;
    for (; i < NSTACKS && size > sizes_[i]; i++)
        ;
    if (i >= NSTACKS)
        log_fatal("could not bucketize %" PRIuPTR " bytes", size);
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

/* bypass interceptors */
REAL_DECL(void *, malloc, size_t n);
REAL_DECL(void *, mmap, void *, size_t, int, int, int, off_t);

static bool
is_initd_()
{
    return mp_.pool.memory != NULL;
}

DICE_WEAK DICE_HIDE void
mempool_init(size_t cap)
{
    if (likely(is_initd_()))
        return;

    memset(&mp_.stack, 0, sizeof(entry_t *) * NSTACKS);
    mp_.allocated     = 0;
    mp_.pool.capacity = cap;
    mp_.pool.next     = 0;
#ifdef DICE_MEMPOOL_USE_MMAP
    mp_.pool.memory   = REAL_FUNCV(mmap, 0)(NULL, cap,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);
    if (mp_.pool.memory == MAP_FAILED)
        log_fatal("could not create mempool");
#else
    mp_.pool.memory   = REAL_FUNCV(malloc, 0)(cap);
    if (mp_.pool.memory == NULL)
        log_fatal("could not create mempool");
    memset(mp_.pool.memory, 0, cap);
#endif
    // caslock already initialized with 0
}

static inline void
mempool_ensure_initd(void)
{
    // assumes protected by lock
    mempool_init((size_t)MEMPOOL_SIZE);
}

DICE_HIDE void mempool_free_(void *ptr);
DICE_HIDE void *mempool_aligned_alloc_(size_t alignment, size_t n);

DICE_HIDE void *
mempool_alloc_(size_t n)
{
    return mempool_aligned_alloc_(1, n);
}

DICE_WEAK void *
mempool_alloc(size_t n)
{
    return mempool_alloc_(n);
}

DICE_HIDE void *
mempool_realloc_(void *ptr, size_t size)
{
    void *p = mempool_alloc_(size);
    if (!p || !ptr)
        return p;
    entry_t *e = (entry_t *)ptr - 1;
    size       = e->size < size ? e->size : size;
    memcpy(p, ptr, size);
    mempool_free_(ptr);
    return p;
}

DICE_WEAK void *
mempool_realloc(void *ptr, size_t size)
{
    return mempool_realloc_(ptr, size);
}

DICE_HIDE void
mempool_free_(void *ptr)
{
    mempool_t *mp = &mp_;
    if (ptr == NULL)
        return;
    entry_t *e      = *((entry_t **)ptr - 1);
    size_t size     = e->size + HEADER_SIZE;
    unsigned bucket = bucketize_(size);
    size            = sizes_[bucket];
    entry_t **stack = &mp->stack[bucket];

    caslock_acquire(&mp->lock);
    mp->allocated -= size;
    if (stack == NULL)
        log_fatal("could not get bucket stack: %" PRIuPTR, size);
    e->next = *stack;
    *stack  = e;
    caslock_release(&mp->lock);
}

DICE_WEAK void
mempool_free(void *ptr)
{
    return mempool_free_(ptr);
}

DICE_HIDE void *
mempool_aligned_alloc_(size_t alignment, size_t sz)
{
    if (!alignment || (alignment & (alignment - 1)))
        log_fatal("mempool misalignment %" PRIuPTR "/%" PRIuPTR, alignment, sz);

    mempool_t *mp   = &mp_;
    entry_t *e      = NULL;
    size_t size     = sz + HEADER_SIZE + alignment - 1;
    unsigned bucket = bucketize_(size);
    size            = sizes_[bucket];
    entry_t **stack = &mp->stack[bucket];
    if (stack == NULL)
        log_fatal("could not get bucket stack: %" PRIuPTR, size);

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

    if (mp->pool.capacity >= mp->pool.next + size) {
        e       = (entry_t *)(mp->pool.memory + mp->pool.next);
        e->next = NULL;
        e->size = sz + alignment - 1;
        mp->pool.next += size;
        mp->allocated += size;
    }
out:
    caslock_release(&mp->lock);
    if (likely(e != NULL)) {
        void *result = (void *)(((size_t)e + HEADER_SIZE + alignment - 1) &
                                ~(alignment - 1));
        *((entry_t **)result - 1) = e;
        return result;
    }
    return NULL;
}

DICE_WEAK void *
mempool_aligned_alloc(size_t alignment, size_t n)
{
    return mempool_aligned_alloc_(alignment, n);
}
