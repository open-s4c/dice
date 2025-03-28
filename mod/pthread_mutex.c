/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/intercept.h>
#include <bingo/interpose.h>
#include <bingo/pthread.h>

INTERPOSE(int, pthread_mutex_lock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex};
    intercept_before(EVENT_MUTEX_LOCK, &ev, 0);
    ev.ret = REAL(pthread_mutex_lock, mutex);
    intercept_after(EVENT_MUTEX_LOCK, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_trylock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex};
    intercept_before(EVENT_MUTEX_TRYLOCK, &ev, 0);
    ev.ret = REAL(pthread_mutex_trylock, mutex);
    intercept_after(EVENT_MUTEX_TRYLOCK, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_mutex_unlock, pthread_mutex_t *mutex)
{
    struct pthread_mutex_event ev = {.mutex = mutex};
    intercept_before(EVENT_MUTEX_UNLOCK, 0, 0);
    ev.ret = REAL(pthread_mutex_unlock, mutex);
    intercept_after(EVENT_MUTEX_UNLOCK, &ev, 0);
    return ev.ret;
}

BINGO_MODULE_INIT()
