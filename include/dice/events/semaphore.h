/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_SEM_H
#define DICE_SEM_H

#include <semaphore.h>

#define EVENT_SEM_POST      70
#define EVENT_SEM_WAIT      71
#define EVENT_SEM_TRYWAIT   72
#define EVENT_SEM_TIMEDWAIT 73

struct sem_post_event {
    const void *pc;
    sem_t *sem;
    int ret;
    int (*func)(sem_t *);
};

struct sem_wait_event {
    const void *pc;
    sem_t *sem;
    int ret;
    int (*func)(sem_t *);
};

struct sem_trywait_event {
    const void *pc;
    sem_t *sem;
    int ret;
    int (*func)(sem_t *);
};

#if !defined(__APPLE__)
struct sem_timedwait_event {
    const void *pc;
    sem_t *sem;
    const struct timespec *abstime;
    int ret;
    int (*func)(sem_t *, const struct timespec *);
};
#endif

#endif /* DICE_SEM_H */
