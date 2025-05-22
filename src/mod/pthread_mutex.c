/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/intercept/pthread.h>
#include <bingo/interpose.h>

INTERPOSE(int, pthread_mutex_lock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_LOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_lock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_LOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_timedlock, pthread_mutex_t *mutex,
          const struct timespec *abstime)
{
    struct pthread_mutex_event ev = {
        .pc      = INTERPOSE_PC,
        .mutex   = mutex,
        .abstime = abstime,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_TIMEDLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_timedlock, mutex, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_TIMEDLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_trylock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_TRYLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_trylock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_TRYLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_unlock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUTEX_UNLOCK, &ev, &md);
    ev.ret = REAL(pthread_mutex_unlock, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUTEX_UNLOCK, &ev, &md);
    return ev.ret;
}

BINGO_MODULE_INIT()
