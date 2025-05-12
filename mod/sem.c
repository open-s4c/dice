/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <semaphore.h>

#include <bingo/intercept/semaphore.h>
#include <bingo/interpose.h>

INTERPOSE(int, sem_post, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_POST, &ev, &md);
    ev.ret = REAL(sem_post, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_POST, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, sem_wait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_WAIT, &ev, &md);
    ev.ret = REAL(sem_wait, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_WAIT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, sem_trywait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_TRYWAIT, &ev, &md);
    ev.ret = REAL(sem_trywait, sem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_TRYWAIT, &ev, &md);
    return ev.ret;
}

#if defined(__linux__)
INTERPOSE(int, sem_timedwait, sem_t *sem, const struct timespec *timeout)
{
    struct sem_event ev = {.sem = sem, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEM_TIMEDWAIT, &ev, &md);
    ev.ret = REAL(sem_timedwait, sem, timeout);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEM_TIMEDWAIT, &ev, &md);
    return ev.ret;
}
#endif

BINGO_MODULE_INIT()
