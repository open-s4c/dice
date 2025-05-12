/*
ap* Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <bingo/intercept/pthread.h>
#include <bingo/interpose.h>

INTERPOSE(int, pthread_rwlock_rdlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_RDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_rdlock, rwlock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_RDLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_timedrdlock, pthread_rwlock_t *rwlock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_event ev = {
        .pc      = INTERPOSE_PC,
        .rwlock  = rwlock,
        .abstime = abstime,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TIMEDRDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_timedrdlock, rwlock, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TIMEDRDLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_tryrdlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TRYRDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_tryrdlock, rwlock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TRYRDLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_wrlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_WRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_wrlock, rwlock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_WRLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_timedwrlock, pthread_rwlock_t *rwlock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_event ev = {
        .pc      = INTERPOSE_PC,
        .rwlock  = rwlock,
        .abstime = abstime,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TIMEDWRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_timedwrlock, rwlock, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TIMEDWRLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_trywrlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TRYWRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_trywrlock, rwlock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TRYWRLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_unlock, pthread_rwlock_t *rwlock)
{
    struct pthread_rwlock_event ev = {.rwlock = rwlock, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_UNLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_unlock, rwlock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_UNLOCK, &ev, &md);
    return ev.ret;
}

BINGO_MODULE_INIT()
