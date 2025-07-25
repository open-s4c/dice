/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
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
};

struct sem_wait_event {
    const void *pc;
    sem_t *sem;
    int ret;
};

struct sem_trywait_event {
    const void *pc;
    sem_t *sem;
    int ret;
};

#if !defined(__APPLE__)
struct sem_timedwait_event {
    const void *pc;
    sem_t *sem;
    const struct timespec *abstime;
    int ret;
};
#endif

#endif /* DICE_SEM_H */
