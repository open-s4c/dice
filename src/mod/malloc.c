/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/malloc.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/mempool_user.h>
#include <errno.h>
#include <string.h>

INTERPOSE(void *, malloc, size_t size)
{
    struct malloc_event ev = {
        .pc   = INTERPOSE_PC,
        .size = size,
        .ret  = 0,
        .func = mempool_user_alloc,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MALLOC, &ev, &md);
    ev.ret = ev.func(ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MALLOC, &ev, &md);
    return ev.ret;
}

static void* mempool_user_calloc(size_t number, size_t size)
{
    size_t total_size = number * size;
    void* ptr = mempool_user_alloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

INTERPOSE(void *, calloc, size_t number, size_t size)
{
    struct calloc_event ev = {
        .pc     = INTERPOSE_PC,
        .number = number,
        .size   = size,
        .ret    = 0,
        .func   = mempool_user_calloc,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CALLOC, &ev, &md);
    ev.ret = ev.func(ev.number, ev.size);
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
        .func = mempool_user_realloc,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REALLOC, &ev, &md);
    ev.ret = ev.func(ev.ptr, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, free, void *ptr)
{
    struct free_event ev = {
        .pc   = INTERPOSE_PC,
        .ptr  = ptr,
        .func = mempool_user_free,
    };
#if defined(__APPLE__)
    // On macOS, if we intercept free when ptr == 0, the program hangs. We still
    // need to investigate why this is happening.
    if (ptr == 0) {
        ev.func(ptr);
        return;
    }
#endif
    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FREE, &ev, &md);
    ev.func(ev.ptr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FREE, &ev, &md);
}

static int mempool_user_posix_memalign(void **ptr, size_t alignment, size_t size)
{
    if (!alignment || (alignment & (alignment - 1)))
        return EINVAL;

    *ptr = mempool_user_aligned_alloc(alignment, size);
    return *ptr ? 0 : ENOMEM;
}

INTERPOSE(int, posix_memalign, void **ptr, size_t alignment, size_t size)
{
    struct posix_memalign_event ev = {
        .pc        = INTERPOSE_PC,
        .ptr       = ptr,
        .alignment = alignment,
        .size      = size,
        .ret       = 0,
        .func      = mempool_user_posix_memalign,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_MEMALIGN, &ev, &md);
    ev.ret = ev.func(ev.ptr, ev.alignment, ev.size);
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
        .func      = mempool_user_aligned_alloc,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, &ev, &md);
    ev.ret = ev.func(ev.alignment, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, memalign, size_t alignment, size_t size)
{
    struct aligned_alloc_event ev = {
        .pc        = INTERPOSE_PC,
        .alignment = alignment,
        .size      = size,
        .ret       = 0,
        .func      = mempool_user_aligned_alloc,
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, &ev, &md);
    ev.ret = ev.func(ev.alignment, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, &ev, &md);
    return ev.ret;
}

INTERPOSE(size_t, malloc_usable_size, void * ptr)
{
    return mempool_user_usable_size(ptr);
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
