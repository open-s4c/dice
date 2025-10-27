/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <semaphore.h>

#include <dice/chains/intercept.h>
#include <dice/events/semaphore.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(int, sem_post, sem_t *sem)
{
    struct sem_post_event ev = {
        .pc  = INTERPOSE_PC,
        .sem = sem,
        .ret = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_POST, &ev, &md);
    ev.ret = REAL(sem_post, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_POST, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, sem_wait, sem_t *sem)
{
    struct sem_wait_event ev = {
        .pc  = INTERPOSE_PC,
        .sem = sem,
        .ret = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_WAIT, &ev, &md);
    ev.ret = REAL(sem_wait, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_WAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, sem_trywait, sem_t *sem)
{
    struct sem_trywait_event ev = {
        .pc  = INTERPOSE_PC,
        .sem = sem,
        .ret = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_TRYWAIT, &ev, &md);
    ev.ret = REAL(sem_trywait, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_TRYWAIT, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, sem_timedwait, sem_t *sem, const struct timespec *abstime)
{
    struct sem_timedwait_event ev = {
        .pc      = INTERPOSE_PC,
        .sem     = sem,
        .abstime = abstime,
        .ret     = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_TIMEDWAIT, &ev, &md);
    ev.ret = REAL(sem_timedwait, sem, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_TIMEDWAIT, &ev, &md);
    return ev.ret;
}
#endif

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_SEM_POST)
PS_ADVERTISE_TYPE(EVENT_SEM_WAIT)
PS_ADVERTISE_TYPE(EVENT_SEM_TRYWAIT)
PS_ADVERTISE_TYPE(EVENT_SEM_TIMEDWAIT)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
