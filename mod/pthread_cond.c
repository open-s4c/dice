/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/intercept/pthread.h>
#include <bingo/interpose.h>

INTERPOSE(int, pthread_cond_wait, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    struct pthread_cond_event ev = {.cond  = cond,
                                    .mutex = mutex,
                                    .pc    = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_WAIT, &ev, &md);
    ev.ret = REAL(pthread_cond_wait, cond, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_WAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_timedwait, pthread_cond_t *cond,
          pthread_mutex_t *mutex, const struct timespec *abstime)
{
    struct pthread_cond_event ev = {.cond    = cond,
                                    .mutex   = mutex,
                                    .abstime = abstime,
                                    .pc      = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_TIMEDWAIT, &ev, &md);
    ev.ret = REAL(pthread_cond_timedwait, cond, mutex, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_TIMEDWAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_signal, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_SIGNAL, &ev, &md);
    ev.ret = REAL(pthread_cond_signal, cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_SIGNAL, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_broadcast, pthread_cond_t *cond)
{
    struct pthread_cond_event ev = {.cond = cond, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_BROADCAST, &ev, &md);
    ev.ret = REAL(pthread_cond_broadcast, cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_BROADCAST, &ev, &md);
    return ev.ret;
}

BINGO_MODULE_INIT()
