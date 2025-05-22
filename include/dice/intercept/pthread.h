/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_PTHREAD_H
#define DICE_PTHREAD_H

#include <pthread.h>

#include <dice/intercept.h>

#define EVENT_THREAD_CREATE      10
#define EVENT_THREAD_JOIN        11
#define EVENT_MUTEX_LOCK         12
#define EVENT_MUTEX_TIMEDLOCK    13
#define EVENT_MUTEX_TRYLOCK      14
#define EVENT_MUTEX_UNLOCK       15
#define EVENT_COND_WAIT          16
#define EVENT_COND_TIMEDWAIT     17
#define EVENT_COND_SIGNAL        18
#define EVENT_COND_BROADCAST     19
#define EVENT_RWLOCK_RDLOCK      20
#define EVENT_RWLOCK_TIMEDRDLOCK 21
#define EVENT_RWLOCK_TRYRDLOCK   22
#define EVENT_RWLOCK_WRLOCK      23
#define EVENT_RWLOCK_TIMEDWRLOCK 24
#define EVENT_RWLOCK_TRYWRLOCK   25
#define EVENT_RWLOCK_UNLOCK      26

struct pthread_create_event {
    const void *pc;
    const pthread_t *thread;
    int ret;
};

struct pthread_join_event {
    const void *pc;
    const pthread_t thread;
    int ret;
};

struct pthread_mutex_event {
    const void *pc;
    const pthread_mutex_t *mutex;
    const struct timespec *abstime;
    int ret;
};

struct pthread_cond_event {
    const void *pc;
    const pthread_cond_t *cond;
    const pthread_mutex_t *mutex;
    const struct timespec *abstime;
    int ret;
};

struct pthread_rwlock_event {
    const void *pc;
    const pthread_rwlock_t *rwlock;
    const struct timespec *abstime;
    int ret;
};

#endif /* DICE_PTHREAD_H */
