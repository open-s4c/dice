/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/intercept.h>
#include <bingo/interpose.h>
#include <bingo/pthread.h>

INTERPOSE(int, pthread_cond_wait, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    struct pthread_cond_event ev = {.cond = cond, .mutex = mutex};
    intercept_before(EVENT_COND_WAIT, &ev, 0);
    ev.ret = REAL(pthread_cond_wait, cond, mutex);
    intercept_after(EVENT_COND_WAIT, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_timedwait, pthread_cond_t *cond,
          pthread_mutex_t *mutex, const struct timespec *abstime)
{
    struct pthread_cond_event ev = {.cond = cond, .mutex = mutex};
    intercept_before(EVENT_COND_TIMEDWAIT, &ev, 0);
    ev.ret = REAL(pthread_cond_timedwait, cond, mutex, abstime);
    intercept_after(EVENT_COND_TIMEDWAIT, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_signal, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond};
    intercept_before(EVENT_COND_SIGNAL, &ev, 0);
    ev.ret = REAL(pthread_cond_signal, cond);
    intercept_after(EVENT_COND_SIGNAL, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_broadcast, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond};
    intercept_before(EVENT_COND_BROADCAST, &ev, 0);
    ev.ret = REAL(pthread_cond_broadcast, cond);
    intercept_after(EVENT_COND_BROADCAST, &ev, 0);
    return ev.ret;
}

BINGO_MODULE_INIT()
