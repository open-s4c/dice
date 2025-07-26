/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DICE_TEST_INTERPOSE
#include <dice/chains/intercept.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>
#include <dice/events/pthread.h>

#define ensure(COND)                                                           \
    {                                                                          \
        if (!(COND)) {                                                         \
            log_fatalf("error: %s\n", #COND);                                  \
        }                                                                      \
    }

static void *symbol;
/* we need to declare this as noinline, otherwise the optimization of the
 * compiler gets rid of the symbol. */
static __attribute__((noinline)) void
enable(void *foo)
{
    symbol = foo;
}
static __attribute__((noinline)) void
disable(void)
{
    symbol = NULL;
}
static inline bool
enabled(void)
{
    return symbol != NULL;
}

void *
real_sym(const char *name, const char *ver)
{
    (void)ver;
    if (!enabled())
        return _real_sym(name, ver);
    return symbol;
}

/* Expects struct to match this:
 *
 * struct pthread_mutex_lock_event {
 *     pthread_mutex_t *mutex;
 *      int  ret;
 * };
 */
struct pthread_mutex_lock_event E_pthread_mutex_lock;
/* Expects struct to match this:
 *
 * struct pthread_mutex_unlock_event {
 *     pthread_mutex_t *mutex;
 *      int  ret;
 * };
 */
struct pthread_mutex_unlock_event E_pthread_mutex_unlock;
/* Expects struct to match this:
 *
 * struct pthread_mutex_trylock_event {
 *     pthread_mutex_t *mutex;
 *      int  ret;
 * };
 */
struct pthread_mutex_trylock_event E_pthread_mutex_trylock;
/* Expects struct to match this:
 *
 * struct pthread_mutex_timedlock_event {
 *     pthread_mutex_t *mutex;
 *     const struct timespec *timeout;
 *      int  ret;
 * };
 */
struct pthread_mutex_timedlock_event E_pthread_mutex_timedlock;

/* mock implementation of functions */
int
fake_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    /* check that every argument is as expected */
    ensure(mutex == E_pthread_mutex_lock.mutex);
    /* return expected value */
 return E_pthread_mutex_lock.ret;
}
int
fake_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    /* check that every argument is as expected */
    ensure(mutex == E_pthread_mutex_unlock.mutex);
    /* return expected value */
 return E_pthread_mutex_unlock.ret;
}
int
fake_pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    /* check that every argument is as expected */
    ensure(mutex == E_pthread_mutex_trylock.mutex);
    /* return expected value */
 return E_pthread_mutex_trylock.ret;
}
int
fake_pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *timeout)
{
    /* check that every argument is as expected */
    ensure(mutex == E_pthread_mutex_timedlock.mutex);
    ensure(timeout == E_pthread_mutex_timedlock.timeout);
    /* return expected value */
 return E_pthread_mutex_timedlock.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_MUTEX_LOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_lock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_lock, mutex);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_MUTEX_LOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_lock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_lock, mutex);
 ASSERT_FIELD_EQ(&E_pthread_mutex_lock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_MUTEX_UNLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_unlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_unlock, mutex);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_MUTEX_UNLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_unlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_unlock, mutex);
 ASSERT_FIELD_EQ(&E_pthread_mutex_unlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_MUTEX_TRYLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_trylock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_trylock, mutex);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_MUTEX_TRYLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_trylock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_trylock, mutex);
 ASSERT_FIELD_EQ(&E_pthread_mutex_trylock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_MUTEX_TIMEDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_timedlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_timedlock, mutex);
    ASSERT_FIELD_EQ(&E_pthread_mutex_timedlock, timeout);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_MUTEX_TIMEDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_mutex_timedlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_mutex_timedlock, mutex);
    ASSERT_FIELD_EQ(&E_pthread_mutex_timedlock, timeout);
 ASSERT_FIELD_EQ(&E_pthread_mutex_timedlock, ret);
})


static void
event_init(void *ptr, size_t n)
{
    char *buf = ptr;
    for (size_t i = 0; i < n; i++)
        buf[i] = rand() % 256;
}

/* test case */

static void
test_pthread_mutex_lock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_mutex_lock, sizeof(struct pthread_mutex_lock_event));
    /* call pthread_mutex_lock with arguments */
    enable(fake_pthread_mutex_lock);
     int  ret =                                   //
                                 pthread_mutex_lock(                                    //
                                     E_pthread_mutex_lock.mutex                                  );
 ensure(ret == E_pthread_mutex_lock.ret);
    disable();
}
static void
test_pthread_mutex_unlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_mutex_unlock, sizeof(struct pthread_mutex_unlock_event));
    /* call pthread_mutex_unlock with arguments */
    enable(fake_pthread_mutex_unlock);
     int  ret =                                   //
                                 pthread_mutex_unlock(                                    //
                                     E_pthread_mutex_unlock.mutex                                  );
 ensure(ret == E_pthread_mutex_unlock.ret);
    disable();
}
static void
test_pthread_mutex_trylock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_mutex_trylock, sizeof(struct pthread_mutex_trylock_event));
    /* call pthread_mutex_trylock with arguments */
    enable(fake_pthread_mutex_trylock);
     int  ret =                                   //
                                 pthread_mutex_trylock(                                    //
                                     E_pthread_mutex_trylock.mutex                                  );
 ensure(ret == E_pthread_mutex_trylock.ret);
    disable();
}
static void
test_pthread_mutex_timedlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_mutex_timedlock, sizeof(struct pthread_mutex_timedlock_event));
    /* call pthread_mutex_timedlock with arguments */
    enable(fake_pthread_mutex_timedlock);
     int  ret =                                   //
                                 pthread_mutex_timedlock(                                    //
                                     E_pthread_mutex_timedlock.mutex,                           //
                                     E_pthread_mutex_timedlock.timeout                                  );
 ensure(ret == E_pthread_mutex_timedlock.ret);
    disable();
}

int
main()
{
    test_pthread_mutex_lock();
    test_pthread_mutex_unlock();
    test_pthread_mutex_trylock();
    test_pthread_mutex_timedlock();
    return 0;
}
