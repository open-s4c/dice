/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_CXA_H
#define DICE_CXA_H

#define EVENT_CXA_GUARD_ACQUIRE 60
#define EVENT_CXA_GUARD_RELEASE 61
#define EVENT_CXA_GUARD_ABORT   62

#define EVENT___CXA_GUARD_ACQUIRE EVENT_CXA_GUARD_ACQUIRE
#define EVENT___CXA_GUARD_RELEASE EVENT_CXA_GUARD_RELEASE
#define EVENT___CXA_GUARD_ABORT   EVENT_CXA_GUARD_ABORT

int __cxa_guard_acquire(void *addr);
int __cxa_guard_release(void *addr);
void __cxa_guard_abort(void *addr);

struct __cxa_guard_acquire_event {
    const void *pc;
    void *addr;
    int ret;
};

struct __cxa_guard_release_event {
    const void *pc;
    void *addr;
    int ret;
};

struct __cxa_guard_abort_event {
    const void *pc;
    void *addr;
};

#endif /* DICE_CXA_H */
