/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_PTHREAD_H
#define DICE_PTHREAD_H

#include <pthread.h>

#define EVENT_PTHREAD_START 1
#define EVENT_PTHREAD_EXIT  2
#define EVENT_PTHREAD_CREATE 3
#define EVENT_PTHREAD_JOIN   4

#define EVENT_PTHREAD_SPIN_LOCK    9
#define EVENT_PTHREAD_SPIN_TRYLOCK 10
#define EVENT_PTHREAD_SPIN_UNLOCK  11

#define EVENT_PTHREAD_MUTEX_LOCK         12
#define EVENT_PTHREAD_MUTEX_TIMEDLOCK    13
#define EVENT_PTHREAD_MUTEX_TRYLOCK      14
#define EVENT_PTHREAD_MUTEX_UNLOCK       15
#define EVENT_PTHREAD_MUTEX_CLOCKLOCK    16
#define EVENT_PTHREAD_COND_WAIT          17
#define EVENT_PTHREAD_COND_TIMEDWAIT     18
#define EVENT_PTHREAD_COND_SIGNAL        19
#define EVENT_PTHREAD_COND_BROADCAST     20
#define EVENT_PTHREAD_COND_CLOCKWAIT     21
#define EVENT_PTHREAD_RWLOCK_RDLOCK      22
#define EVENT_PTHREAD_RWLOCK_TIMEDRDLOCK 23
#define EVENT_PTHREAD_RWLOCK_TRYRDLOCK   24
#define EVENT_PTHREAD_RWLOCK_WRLOCK      25
#define EVENT_PTHREAD_RWLOCK_TIMEDWRLOCK 26
#define EVENT_PTHREAD_RWLOCK_TRYWRLOCK   27
#define EVENT_PTHREAD_RWLOCK_UNLOCK      28

#if !defined(__APPLE__) && !defined(__NetBSD__) &&                             \
        !(defined(__linux__) && !defined(__GLIBC__)) ||                        \
    defined(__OHOS__)
    // pthread_cond_clockwait and pthread_mutex_clocklock are not available in
    // macos, netbsd and alpine
    #define HAVE_PTHREAD_CLOCKWAIT_LOCK
#endif

struct pthread_create_event {
    const void *pc;
    pthread_t *thread;
    const pthread_attr_t *attr;
    void *(*run)(void *);
    void *arg;
    int ret;
    int (*func)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
};

struct pthread_join_event {
    const void *pc;
    pthread_t thread;
    void **ptr;
    int ret;
    int (*func)(pthread_t, void **);
};

struct pthread_exit_event {
    const void *pc;
    void *ptr;
    void (*func)(void *);
};

struct pthread_mutex_lock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
    int (*func)(pthread_mutex_t *);
};

struct pthread_mutex_trylock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
    int (*func)(pthread_mutex_t *);
};

struct pthread_mutex_unlock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    int ret;
    int (*func)(pthread_mutex_t *);
};

struct pthread_mutex_timedlock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    const struct timespec *timeout;
    int ret;
    int (*func)(pthread_mutex_t *, const struct timespec *);
};

#if defined(HAVE_PTHREAD_CLOCKWAIT_LOCK)
struct pthread_mutex_clocklock_event {
    const void *pc;
    pthread_mutex_t *mutex;
    clockid_t clock_id;
    const struct timespec *abstime;
    int ret;
    int (*func)(pthread_mutex_t *, clockid_t, const struct timespec *);
};
#endif

struct pthread_cond_wait_event {
    const void *pc;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int ret;
    int (*func)(pthread_cond_t *, pthread_mutex_t *);
};

struct pthread_cond_timedwait_event {
    const void *pc;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    const struct timespec *abstime;
    int ret;
    int (*func)(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
};

struct pthread_cond_signal_event {
    const void *pc;
    pthread_cond_t *cond;
    int ret;
    int (*func)(pthread_cond_t *);
};

struct pthread_cond_broadcast_event {
    const void *pc;
    pthread_cond_t *cond;
    int ret;
    int (*func)(pthread_cond_t *);
};

#if defined(HAVE_PTHREAD_CLOCKWAIT_LOCK)
struct pthread_cond_clockwait_event {
    const void *pc;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    clockid_t clock_id;
    const struct timespec *abstime;
    int ret;
    int (*func)(pthread_cond_t *, pthread_mutex_t *, clockid_t,
                const struct timespec *);
};
#endif

struct pthread_rwlock_rdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
    int (*func)(pthread_rwlock_t *);
};

struct pthread_rwlock_tryrdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
    int (*func)(pthread_rwlock_t *);
};
struct pthread_rwlock_timedrdlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    const struct timespec *abstime;
    int ret;
    int (*func)(pthread_rwlock_t *, const struct timespec *);
};
struct pthread_rwlock_wrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
    int (*func)(pthread_rwlock_t *);
};
struct pthread_rwlock_trywrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
    int (*func)(pthread_rwlock_t *);
};
struct pthread_rwlock_timedwrlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    const struct timespec *abstime;
    int ret;
    int (*func)(pthread_rwlock_t *, const struct timespec *);
};
struct pthread_rwlock_unlock_event {
    const void *pc;
    pthread_rwlock_t *lock;
    int ret;
    int (*func)(pthread_rwlock_t *);
};

#if !defined(__APPLE__)
struct pthread_spin_lock_event {
    const void *pc;
    pthread_spinlock_t *lock;
    int ret;
    int (*func)(pthread_spinlock_t *);
};

struct pthread_spin_trylock_event {
    const void *pc;
    pthread_spinlock_t *lock;
    int ret;
    int (*func)(pthread_spinlock_t *);
};

struct pthread_spin_unlock_event {
    const void *pc;
    pthread_spinlock_t *lock;
    int ret;
    int (*func)(pthread_spinlock_t *);
};
#endif

#endif /* DICE_PTHREAD_H */
