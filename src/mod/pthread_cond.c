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

INTERPOSE(int, pthread_cond_wait, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    struct pthread_cond_wait_event ev = {
        .pc    = INTERPOSE_PC,
        .cond  = cond,
        .mutex = mutex,
        .ret   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_WAIT, &ev, &md);
    ev.ret = REALP(pthread_cond_wait, cond, mutex);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_WAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_timedwait, pthread_cond_t *cond,
          pthread_mutex_t *mutex, const struct timespec *abstime)
{
    struct pthread_cond_timedwait_event ev = {
        .pc      = INTERPOSE_PC,
        .cond    = cond,
        .mutex   = mutex,
        .abstime = abstime,
        .ret     = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_TIMEDWAIT, &ev, &md);
    ev.ret = REALP(pthread_cond_timedwait, cond, mutex, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_TIMEDWAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_signal, pthread_cond_t *cond)
{
    struct pthread_cond_signal_event ev = {
        .pc   = INTERPOSE_PC,
        .cond = cond,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_SIGNAL, &ev, &md);
    ev.ret = REALP(pthread_cond_signal, cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_SIGNAL, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_broadcast, pthread_cond_t *cond)
{
    struct pthread_cond_broadcast_event ev = {
        .pc   = INTERPOSE_PC,
        .cond = cond,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_BROADCAST, &ev, &md);
    ev.ret = REALP(pthread_cond_broadcast, cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_BROADCAST, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_WAIT)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_TIMEDWAIT)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_SIGNAL)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_BROADCAST)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
