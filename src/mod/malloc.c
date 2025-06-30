/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <dice/chains/intercept.h>
#include <dice/events/malloc.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(void *, malloc, size_t size)
{
    struct malloc_event ev = {
        .pc   = INTERPOSE_PC,
        .size = size,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MALLOC, &ev, &md);
    ev.ret = REAL(malloc, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, calloc, size_t number, size_t size)
{
    struct calloc_event ev = {
        .pc     = INTERPOSE_PC,
        .number = number,
        .size   = size,
        .ret    = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CALLOC, &ev, &md);
    ev.ret = REAL(calloc, number, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, realloc, void *ptr, size_t size)
{
    struct realloc_event ev = {
        .pc   = INTERPOSE_PC,
        .ptr  = ptr,
        .size = size,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REALLOC, &ev, &md);
    ev.ret = REAL(realloc, ptr, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, free, void *ptr)
{
    struct free_event ev = {
        .pc  = INTERPOSE_PC,
        .ptr = ptr,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FREE, &ev, &md);
    REAL(free, ptr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FREE, &ev, &md);
}

INTERPOSE(int, posix_memalign, void **ptr, size_t alignment, size_t size)
{
    struct posix_memalign_event ev = {
        .pc        = INTERPOSE_PC,
        .ptr       = ptr,
        .alignment = alignment,
        .size      = size,
        .ret       = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_MEMALIGN, &ev, &md);
    ev.ret = REAL(posix_memalign, ptr, alignment, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POSIX_MEMALIGN, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, aligned_alloc, size_t alignment, size_t size)
{
    struct aligned_alloc_event ev = {
        .pc        = INTERPOSE_PC,
        .alignment = alignment,
        .size      = size,
        .ret       = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, &ev, &md);
    ev.ret = REAL(aligned_alloc, alignment, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, &ev, &md);
    return ev.ret;
}

DICE_MODULE_INIT()
