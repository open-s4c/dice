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
    struct memcpy_event ev;
    ev.pc   = INTERPOSE_PC;
    ev.dest = dest;
    ev.src  = src;
    ev.num  = num;
    ev.ret  = 0;
    ev.func = REAL_FUNC(memcpy);

    metadata_t md;
    md.drop = false;
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMCPY, &ev, &md);
    ev.ret = ev.func(dest, src, num);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMCPY, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, memmove, void *dest, const void *src, size_t count)
{
    struct memmove_event ev;
    ev.pc    = INTERPOSE_PC;
    ev.dest  = dest;
    ev.src   = src;
    ev.count = count;
    ev.ret   = 0;
    ev.func  = REAL_FUNC(memmove);

    metadata_t md;
    md.drop = false;
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMMOVE, &ev, &md);
    ev.ret = ev.func(dest, src, count);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMMOVE, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, memset, void *ptr, int value, size_t num)
{
    // struct memset_event ev = {
    //     .pc    = INTERPOSE_PC,
    //     .ptr   = ptr,
    //     .value = value,
    //     .num   = num,
    //     .ret   = 0,
    //     .func  = REAL_FUNC(memset),
    // };
    struct memset_event ev;
    ev.pc    = INTERPOSE_PC;
    ev.ptr   = ptr;
    ev.value = value;
    ev.num   = num;
    ev.ret   = 0;
    ev.func  = REAL_FUNC(memset);

    metadata_t md;
    md.drop = false;
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MEMSET, &ev, &md);
    ev.ret = ev.func(ptr, value, num);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MEMSET, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_MEMCPY)
PS_ADVERTISE_TYPE(EVENT_MEMSET)
PS_ADVERTISE_TYPE(EVENT_MEMMOVE)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
