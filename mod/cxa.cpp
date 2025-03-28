/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
extern "C" {
#include <bingo/cxa.h>
#include <bingo/intercept.h>
#include <bingo/interpose.h>
int __cxa_guard_acquire(void *addr);
int __cxa_guard_release(void *addr);
void __cxa_guard_abort(void *addr);
}

INTERPOSE(int, __cxa_guard_acquire, void *addr)
{
    struct cxa_event ev = {.addr = addr};
    intercept_before(EVENT_CXA_GUARD_ACQUIRE, &ev, 0);
    ev.ret = REAL(__cxa_guard_acquire, addr);
    intercept_after(EVENT_CXA_GUARD_ACQUIRE, &ev, 0);
    return ev.ret;
}

INTERPOSE(void, __cxa_guard_release, void *addr)
{
    struct cxa_event ev = {.addr = addr};
    intercept_before(EVENT_CXA_GUARD_RELEASE, &ev, 0);
    ev.ret = REAL(__cxa_guard_release, addr);
    intercept_after(EVENT_CXA_GUARD_RELEASE, &ev, 0);
}

INTERPOSE(void, __cxa_guard_abort, void *addr)
{
    struct cxa_event ev = {.addr = addr};
    intercept_before(EVENT_CXA_GUARD_ABORT, &ev, 0);
    REAL(__cxa_guard_abort, addr);
    intercept_before(EVENT_CXA_GUARD_ABORT, &ev, 0);
}

BINGO_MODULE_INIT()
