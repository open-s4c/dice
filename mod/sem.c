/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <semaphore.h>

#include <bingo/intercept.h>
#include <bingo/interpose.h>
#include <bingo/semaphore.h>

INTERPOSE(int, sem_post, sem_t *sem)
{
    struct sem_event ev = {.sem = sem};
    intercept_before(EVENT_SEM_POST, &ev, 0);
    ev.ret = REAL(sem_post, sem);
    intercept_after(EVENT_SEM_POST, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, sem_wait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem};
    intercept_before(EVENT_SEM_WAIT, &ev, 0);
    ev.ret = REAL(sem_wait, sem);
    intercept_after(EVENT_SEM_WAIT, &ev, 0);
    return ev.ret;
}

INTERPOSE(int, sem_trywait, sem_t *sem)
{
    struct sem_event ev = {.sem = sem};
    intercept_before(EVENT_SEM_TRYWAIT, &ev, 0);
    ev.ret = REAL(sem_trywait, sem);
    intercept_after(EVENT_SEM_TRYWAIT, &ev, 0);
    return ev.ret;
}

#if defined(__linux__)
INTERPOSE(int, sem_timedwait, sem_t *sem, const struct timespec *timeout)
{
    struct sem_event ev = {.sem = sem};
    intercept_before(EVENT_SEM_TIMEDWAIT, &ev, 0);
    ev.ret = REAL(sem_timedwait, sem, timeout);
    intercept_after(EVENT_SEM_TIMEDWAIT, &ev, 0);
    return ev.ret;
}
#endif

BINGO_MODULE_INIT()
