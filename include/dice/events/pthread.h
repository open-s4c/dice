/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_PTHREAD_H
#define DICE_PTHREAD_H

#include <pthread.h>

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

#define EVENT_PTHREAD_CREATE EVENT_THREAD_CREATE
#define EVENT_PTHREAD_JOIN   EVENT_THREAD_JOIN

#define EVENT_PTHREAD_MUTEX_LOCK      EVENT_MUTEX_LOCK
#define EVENT_PTHREAD_MUTEX_TRYLOCK   EVENT_MUTEX_TRYLOCK
#define EVENT_PTHREAD_MUTEX_TIMEDLOCK EVENT_MUTEX_TIMEDLOCK
#define EVENT_PTHREAD_MUTEX_UNLOCK    EVENT_MUTEX_UNLOCK

#define EVENT_PTHREAD_COND_WAIT      EVENT_COND_WAIT
#define EVENT_PTHREAD_COND_TIMEDWAIT EVENT_COND_TIMEDWAIT
#define EVENT_PTHREAD_COND_SIGNAL    EVENT_COND_SIGNAL
#define EVENT_PTHREAD_COND_BROADCAST EVENT_COND_BROADCAST

#define EVENT_PTHREAD_RWLOCK_RDLOCK      EVENT_RWLOCK_RDLOCK
#define EVENT_PTHREAD_RWLOCK_TRYRDLOCK   EVENT_RWLOCK_TRYRDLOCK
#define EVENT_PTHREAD_RWLOCK_TIMEDRDLOCK EVENT_RWLOCK_TIMEDRDLOCK
#define EVENT_PTHREAD_RWLOCK_WRLOCK      EVENT_RWLOCK_WRLOCK
#define EVENT_PTHREAD_RWLOCK_TRYWRLOCK   EVENT_RWLOCK_TRYWRLOCK
#define EVENT_PTHREAD_RWLOCK_TIMEDWRLOCK EVENT_RWLOCK_TIMEDWRLOCK
#define EVENT_PTHREAD_RWLOCK_UNLOCK      EVENT_RWLOCK_UNLOCK

struct pthread_create_event {
    const void *pc;
    pthread_t *thread;
    const pthread_attr_t *attr;
    void *(*run)(void *);
    void *arg;
    int ret;
};

struct pthread_join_event {
    const void *pc;
    pthread_t thread;
    void **ptr;
    int ret;
};

struct pthread_exit_event {
    const void *pc;
    void *ptr;
};

struct pthread_mutex_lock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
};

struct pthread_mutex_trylock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
};

struct pthread_mutex_unlock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
};

struct pthread_mutex_timedlock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    const struct timespec *timeout;
    int ret;
};

struct pthread_cond_wait_event {
    const void *pc;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int ret;
};

struct pthread_cond_timedwait_event {
    const void *pc;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    const struct timespec *abstime;
    int ret;
};

struct pthread_cond_signal_event {
    const void *pc;
    pthread_cond_t *cond;
    int ret;
};

struct pthread_cond_broadcast_event {
    const void *pc;
    pthread_cond_t *cond;
    int ret;
};

struct pthread_rwlock_rdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
};

struct pthread_rwlock_tryrdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
};
struct pthread_rwlock_timedrdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    const struct timespec *abstime;
    int ret;
};
struct pthread_rwlock_wrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
};
struct pthread_rwlock_trywrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
};
struct pthread_rwlock_timedwrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    const struct timespec *abstime;
    int ret;
};
struct pthread_rwlock_unlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
};

#endif /* DICE_PTHREAD_H */
