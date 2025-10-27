/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(int, pthread_rwlock_rdlock, pthread_rwlock_t *lock)
{
    struct pthread_rwlock_rdlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_RDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_rdlock, lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_RDLOCK, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, pthread_rwlock_timedrdlock, pthread_rwlock_t *lock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_timedrdlock_event ev = {
        .pc      = INTERPOSE_PC,
        .lock    = lock,
        .abstime = abstime,
        .ret     = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TIMEDRDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_timedrdlock, lock, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TIMEDRDLOCK, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, pthread_rwlock_tryrdlock, pthread_rwlock_t *lock)
{
    struct pthread_rwlock_tryrdlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TRYRDLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_tryrdlock, lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TRYRDLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_wrlock, pthread_rwlock_t *lock)
{
    struct pthread_rwlock_wrlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_WRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_wrlock, lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_WRLOCK, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, pthread_rwlock_timedwrlock, pthread_rwlock_t *lock,
          const struct timespec *abstime)
{
    struct pthread_rwlock_timedwrlock_event ev = {
        .pc      = INTERPOSE_PC,
        .lock    = lock,
        .abstime = abstime,
        .ret     = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TIMEDWRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_timedwrlock, lock, abstime);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TIMEDWRLOCK, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, pthread_rwlock_trywrlock, pthread_rwlock_t *lock)
{
    struct pthread_rwlock_trywrlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_TRYWRLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_trywrlock, lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_TRYWRLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_rwlock_unlock, pthread_rwlock_t *lock)
{
    struct pthread_rwlock_unlock_event ev = {
        .pc   = INTERPOSE_PC,
        .lock = lock,
        .ret  = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_RWLOCK_UNLOCK, &ev, &md);
    ev.ret = REAL(pthread_rwlock_unlock, lock);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_RWLOCK_UNLOCK, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_RDLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_TIMEDRDLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_TRYRDLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_WRLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_TIMEDWRLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_TRYWRLOCK)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_RWLOCK_UNLOCK)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
