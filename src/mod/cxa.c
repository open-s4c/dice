/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include "dice/module.h"
#include <dice/chains/intercept.h>
#include <dice/events/cxa.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(int, __cxa_guard_acquire, void *addr)
{
    struct __cxa_guard_acquire_event ev = {
        .pc   = INTERPOSE_PC,
        .addr = addr,
        .ret  = 0,
        .func = REAL_FUNC(__cxa_guard_acquire),
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_ACQUIRE, &ev, &md);
    ev.ret = ev.func(addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_ACQUIRE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, __cxa_guard_release, void *addr)
{
    struct __cxa_guard_release_event ev = {
        .pc   = INTERPOSE_PC,
        .addr = addr,
        .ret  = 0,
        .func = REAL_FUNC(__cxa_guard_release),
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_RELEASE, &ev, &md);
    ev.ret = ev.func(addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_RELEASE, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, __cxa_guard_abort, void *addr)
{
    struct __cxa_guard_abort_event ev = {
        .pc   = INTERPOSE_PC,
        .addr = addr,
        .func = REAL_FUNC(__cxa_guard_abort),
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_ABORT, &ev, &md);
    ev.func(addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_ABORT, &ev, &md);
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT___CXA_GUARD_ACQUIRE)
PS_ADVERTISE_TYPE(EVENT___CXA_GUARD_RELEASE)
PS_ADVERTISE_TYPE(EVENT___CXA_GUARD_ABORT)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
