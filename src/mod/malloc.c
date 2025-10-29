/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
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
        .func = REAL_FUNC(malloc),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MALLOC, &ev, &md);
    ev.ret = ev.func(size);
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
        .func   = REAL_FUNC(calloc),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CALLOC, &ev, &md);
    ev.ret = ev.func(number, size);
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
        .func = REAL_FUNC(realloc),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REALLOC, &ev, &md);
    ev.ret = ev.func(ptr, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, free, void *ptr)
{
    struct free_event ev = {
        .pc   = INTERPOSE_PC,
        .ptr  = ptr,
        .func = REAL_FUNC(free),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FREE, &ev, &md);
    ev.func(ptr);
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
        .func      = REAL_FUNC(posix_memalign),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_MEMALIGN, &ev, &md);
    ev.ret = ev.func(ptr, alignment, size);
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
        .func      = REAL_FUNC(aligned_alloc),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, &ev, &md);
    ev.ret = ev.func(alignment, size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_MALLOC)
PS_ADVERTISE_TYPE(EVENT_CALLOC)
PS_ADVERTISE_TYPE(EVENT_REALLOC)
PS_ADVERTISE_TYPE(EVENT_FREE)
PS_ADVERTISE_TYPE(EVENT_POSIX_MEMALIGN)
PS_ADVERTISE_TYPE(EVENT_ALIGNED_ALLOC)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
