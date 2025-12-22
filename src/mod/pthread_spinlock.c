/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(int, pthread_spin_lock, pthread_spinlock_t *lock)
{
    struct pthread_spin_lock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
        .func = REAL_FUNC(pthread_spin_lock),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SPIN_LOCK, &ev, &md);
    ev.ret = ev.func(ev.lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SPIN_LOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_spin_trylock, pthread_spinlock_t *lock)
{
    struct pthread_spin_trylock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
        .func = REAL_FUNC(pthread_spin_trylock),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SPIN_TRYLOCK, &ev, &md);
    ev.ret = ev.func(ev.lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SPIN_TRYLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_spin_unlock, pthread_spinlock_t *lock)
{
    struct pthread_spin_unlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
        .func = REAL_FUNC(pthread_spin_unlock),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SPIN_UNLOCK, &ev, &md);
    ev.ret = ev.func(ev.lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SPIN_UNLOCK, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_SPIN_LOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_SPIN_TRYLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_SPIN_UNLOCK)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
