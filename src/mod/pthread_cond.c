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

INTERPOSE(int, pthread_cond_wait, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    struct pthread_cond_wait_event ev = {
        .pc    = INTERPOSE_PC,
        .cond  = cond,
        .mutex = mutex,
        .ret   = 0,
        .func  = REAL_FUNCP(pthread_cond_wait),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_WAIT, &ev, &md);
    ev.ret = ev.func(ev.cond, ev.mutex);
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
        .func    = REAL_FUNCP(pthread_cond_timedwait),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_TIMEDWAIT, &ev, &md);
    ev.ret = ev.func(ev.cond, ev.mutex, ev.abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_TIMEDWAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_signal, pthread_cond_t *cond)
{
    struct pthread_cond_signal_event ev = {
        .pc   = INTERPOSE_PC,
        .cond = cond,
        .ret  = 0,
        .func = REAL_FUNCP(pthread_cond_signal),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_SIGNAL, &ev, &md);
    ev.ret = ev.func(ev.cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_SIGNAL, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_cond_broadcast, pthread_cond_t *cond)
{
    struct pthread_cond_broadcast_event ev = {
        .pc   = INTERPOSE_PC,
        .cond = cond,
        .ret  = 0,
        .func = REAL_FUNCP(pthread_cond_broadcast),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_BROADCAST, &ev, &md);
    ev.ret = ev.func(ev.cond);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_BROADCAST, &ev, &md);
    return ev.ret;
}

#if defined(HAVE_PTHREAD_CLOCKWAIT_LOCK)
INTERPOSE(int, pthread_cond_clockwait, pthread_cond_t *restrict cond,
          pthread_mutex_t *restrict mutex, clockid_t clock_id,
          const struct timespec *restrict abstime)
{
    struct pthread_cond_clockwait_event ev = {
        .pc       = INTERPOSE_PC,
        .cond     = cond,
        .mutex    = mutex,
        .clock_id = clock_id,
        .abstime  = abstime,
        .ret      = 0,
        .func     = REAL_FUNC(pthread_cond_clockwait),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_COND_CLOCKWAIT, &ev, &md);
    ev.ret = ev.func(ev.cond, ev.mutex, ev.clock_id, ev.abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_COND_CLOCKWAIT, &ev, &md);
    return ev.ret;
}
#endif

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_WAIT)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_TIMEDWAIT)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_SIGNAL)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_BROADCAST)
#ifdef HAVE_PTHREAD_CLOCKWAIT_LOCK
PS_ADVERTISE_TYPE(EVENT_PTHREAD_COND_CLOCKWAIT)
#endif

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
