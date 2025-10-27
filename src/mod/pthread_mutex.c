/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(int, pthread_mutex_lock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_lock_event ev = {
        .pc    = INTERPOSE_PC,
        .mutex = mutex,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_LOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_lock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_LOCK, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, pthread_mutex_timedlock, pthread_mutex_t *mutex,
          const struct timespec *timeout)
{
    struct pthread_mutex_timedlock_event ev = {
        .pc      = INTERPOSE_PC,
        .mutex   = mutex,
        .timeout = timeout,
        .ret     = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_TIMEDLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_timedlock, mutex, timeout);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_TIMEDLOCK, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, pthread_mutex_trylock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_trylock_event ev = {
        .pc    = INTERPOSE_PC,
        .mutex = mutex,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_TRYLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_trylock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_TRYLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_unlock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_unlock_event ev = {
        .pc    = INTERPOSE_PC,
        .mutex = mutex,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_UNLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_unlock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_UNLOCK, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_MUTEX_LOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_MUTEX_TIMEDLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_MUTEX_TRYLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_MUTEX_UNLOCK)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
