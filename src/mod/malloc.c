/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept/malloc.h>
#include <bingo/interpose.h>

INTERPOSE(void *, malloc, size_t n)
{
    struct malloc_event ev = {.size = n, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MALLOC, &ev, &md);
    ev.ptr = REAL(malloc, n);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MALLOC, &ev, &md);
    return ev.ptr;
}

INTERPOSE(void *, calloc, size_t n, size_t s)
{
    struct malloc_event ev = {.size = n * s, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CALLOC, &ev, &md);
    ev.ptr = REAL(calloc, n, s);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CALLOC, &ev, &md);
    return ev.ptr;
}

INTERPOSE(void *, realloc, void *p, size_t n)
{
    struct malloc_event ev = {.ptr = p, .size = n, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REALLOC, &ev, &md);
    ev.ptr = REAL(realloc, p, n);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REALLOC, &ev, &md);
    return ev.ptr;
}

INTERPOSE(void, free, void *p)
{
    struct malloc_event ev = {.ptr = p, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FREE, &ev, &md);
    REAL(free, p);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FREE, &ev, &md);
}

INTERPOSE(int, posix_memalign, void **memptr, size_t alignment, size_t size)
{
    struct malloc_event ev = {.ptr  = (void *)memptr,
                              .size = size,
                              .pc   = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_MEMALIGN, &ev, &md);
    ev.ret = REAL(posix_memalign, memptr, alignment, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POSIX_MEMALIGN, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, aligned_alloc, size_t alignment, size_t size)
{
    struct malloc_event ev = {.size = size, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, &ev, &md);
    ev.ptr = REAL(aligned_alloc, alignment, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, &ev, &md);
    return ev.ptr;
}

BINGO_MODULE_INIT()
