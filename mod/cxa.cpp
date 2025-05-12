/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

extern "C" {
#include <bingo/intercept/cxa.h>
#include <bingo/interpose.h>
#include <bingo/pubsub.h>

int __cxa_guard_acquire(void *addr);
int __cxa_guard_release(void *addr);
void __cxa_guard_abort(void *addr);
}

INTERPOSE(int, __cxa_guard_acquire, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr, .ret = 0};
    metadata_t md       = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_ACQUIRE, &ev, &md);
    ev.ret = REAL(__cxa_guard_acquire, addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_ACQUIRE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, __cxa_guard_release, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr, .ret = 0};
    metadata_t md       = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_RELEASE, &ev, &md);
    ev.ret = REAL(__cxa_guard_release, addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_RELEASE, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, __cxa_guard_abort, void *addr)
{
    struct cxa_event ev = {.pc = INTERPOSE_PC, .addr = addr, .ret = 0};
    metadata_t md       = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CXA_GUARD_ABORT, &ev, &md);
    REAL(__cxa_guard_abort, addr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CXA_GUARD_ABORT, &ev, &md);
}

BINGO_MODULE_INIT()
