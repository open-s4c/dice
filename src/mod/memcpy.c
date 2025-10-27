/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/memcpy.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(void *, memcpy, void *dest, const void *src, size_t num)
{
    struct memcpy_event ev = {
        .pc   = INTERPOSE_PC,
        .dest = dest,
        .src  = src,
        .num  = num,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMCPY, &ev, &md);
    ev.ret = REAL(memcpy, dest, src, num);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMCPY, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, memmove, void *dest, const void *src, size_t count)
{
    struct memmove_event ev = {
        .pc    = INTERPOSE_PC,
        .dest  = dest,
        .src   = src,
        .count = count,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMMOVE, &ev, &md);
    ev.ret = REAL(memmove, dest, src, count);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMMOVE, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, memset, void *ptr, int value, size_t num)
{
    struct memset_event ev = {
        .pc    = INTERPOSE_PC,
        .ptr   = ptr,
        .value = value,
        .num   = num,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMSET, &ev, &md);
    ev.ret = REAL(memset, ptr, value, num);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMSET, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_MEMCPY)
PS_ADVERTISE_TYPE(EVENT_MEMSET)
PS_ADVERTISE_TYPE(EVENT_MEMMOVE)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
