/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept.h>
#include <bingo/interpose.h>
#include <bingo/malloc.h>

INTERPOSE(void *, malloc, size_t n)
{
    struct malloc_event ev = {.size = n};
    intercept_before(EVENT_MALLOC, &ev, 0);
    ev.addr = REAL(malloc, n);
    intercept_after(EVENT_MALLOC, &ev, 0);
    return ev.addr;
}

INTERPOSE(void *, calloc, size_t n, size_t s)
{
    struct malloc_event ev = {.size = n * s};
    intercept_before(EVENT_CALLOC, &ev, 0);
    ev.addr = REAL(calloc, n, s);
    intercept_after(EVENT_CALLOC, &ev, 0);
    return ev.addr;
}

INTERPOSE(void *, realloc, void *p, size_t n)
{
    struct malloc_event ev = {.addr = p, .size = n};
    intercept_before(EVENT_REALLOC, &ev, 0);
    ev.addr = REAL(realloc, p, n);
    intercept_after(EVENT_REALLOC, &ev, 0);
    return ev.addr;
}

INTERPOSE(void, free, void *p)
{
    struct malloc_event ev = {.addr = p};
    intercept_before(EVENT_FREE, &ev, 0);
    REAL(free, p);
    intercept_after(EVENT_FREE, &ev, 0);
}

INTERPOSE(int, posix_memalign, void **memptr, size_t alignment, size_t size)
{
    struct malloc_event ev = {.addr = (void *)memptr, .size = size};
    intercept_before(EVENT_POSIX_MEMALIGN, &ev, 0);
    ev.ret = REAL(posix_memalign, memptr, alignment, size);
    intercept_after(EVENT_POSIX_MEMALIGN, &ev, 0);
    return ev.ret;
}

INTERPOSE(void *, aligned_alloc, size_t alignment, size_t size)
{
    struct malloc_event ev = {.size = size};
    intercept_before(EVENT_ALIGNED_ALLOC, &ev, 0);
    ev.addr = REAL(aligned_alloc, alignment, size);
    intercept_after(EVENT_ALIGNED_ALLOC, &ev, 0);
    return ev.addr;
}

BINGO_MODULE_INIT()
