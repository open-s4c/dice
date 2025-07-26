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
 * struct pthread_rwlock_rdlock_event {
 *     pthread_rwlock_t *lock;
 *      int  ret;
 * };
 */
struct pthread_rwlock_rdlock_event E_pthread_rwlock_rdlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_tryrdlock_event {
 *     pthread_rwlock_t *lock;
 *      int  ret;
 * };
 */
struct pthread_rwlock_tryrdlock_event E_pthread_rwlock_tryrdlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_timedrdlock_event {
 *     pthread_rwlock_t *lock;
 *     const struct timespec *abstime;
 *      int  ret;
 * };
 */
struct pthread_rwlock_timedrdlock_event E_pthread_rwlock_timedrdlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_wrlock_event {
 *     pthread_rwlock_t *lock;
 *      int  ret;
 * };
 */
struct pthread_rwlock_wrlock_event E_pthread_rwlock_wrlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_trywrlock_event {
 *     pthread_rwlock_t *lock;
 *      int  ret;
 * };
 */
struct pthread_rwlock_trywrlock_event E_pthread_rwlock_trywrlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_timedwrlock_event {
 *     pthread_rwlock_t *lock;
 *     const struct timespec *abstime;
 *      int  ret;
 * };
 */
struct pthread_rwlock_timedwrlock_event E_pthread_rwlock_timedwrlock;
/* Expects struct to match this:
 *
 * struct pthread_rwlock_unlock_event {
 *     pthread_rwlock_t *lock;
 *      int  ret;
 * };
 */
struct pthread_rwlock_unlock_event E_pthread_rwlock_unlock;

/* mock implementation of functions */
int
fake_pthread_rwlock_rdlock(pthread_rwlock_t *lock)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_rdlock.lock);
    /* return expected value */
 return E_pthread_rwlock_rdlock.ret;
}
int
fake_pthread_rwlock_tryrdlock(pthread_rwlock_t *lock)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_tryrdlock.lock);
    /* return expected value */
 return E_pthread_rwlock_tryrdlock.ret;
}
int
fake_pthread_rwlock_timedrdlock(pthread_rwlock_t *lock, const struct timespec *abstime)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_timedrdlock.lock);
    ensure(abstime == E_pthread_rwlock_timedrdlock.abstime);
    /* return expected value */
 return E_pthread_rwlock_timedrdlock.ret;
}
int
fake_pthread_rwlock_wrlock(pthread_rwlock_t *lock)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_wrlock.lock);
    /* return expected value */
 return E_pthread_rwlock_wrlock.ret;
}
int
fake_pthread_rwlock_trywrlock(pthread_rwlock_t *lock)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_trywrlock.lock);
    /* return expected value */
 return E_pthread_rwlock_trywrlock.ret;
}
int
fake_pthread_rwlock_timedwrlock(pthread_rwlock_t *lock, const struct timespec *abstime)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_timedwrlock.lock);
    ensure(abstime == E_pthread_rwlock_timedwrlock.abstime);
    /* return expected value */
 return E_pthread_rwlock_timedwrlock.ret;
}
int
fake_pthread_rwlock_unlock(pthread_rwlock_t *lock)
{
    /* check that every argument is as expected */
    ensure(lock == E_pthread_rwlock_unlock.lock);
    /* return expected value */
 return E_pthread_rwlock_unlock.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_RDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_rdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_rdlock, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_RDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_rdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_rdlock, lock);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_rdlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_TRYRDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_tryrdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_tryrdlock, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_TRYRDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_tryrdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_tryrdlock, lock);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_tryrdlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_TIMEDRDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_timedrdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedrdlock, lock);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedrdlock, abstime);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_TIMEDRDLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_timedrdlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedrdlock, lock);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedrdlock, abstime);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_timedrdlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_WRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_wrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_wrlock, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_WRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_wrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_wrlock, lock);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_wrlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_TRYWRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_trywrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_trywrlock, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_TRYWRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_trywrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_trywrlock, lock);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_trywrlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_TIMEDWRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_timedwrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedwrlock, lock);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedwrlock, abstime);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_TIMEDWRLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_timedwrlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedwrlock, lock);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_timedwrlock, abstime);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_timedwrlock, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_RWLOCK_UNLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_unlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_unlock, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_RWLOCK_UNLOCK, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_rwlock_unlock_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_rwlock_unlock, lock);
 ASSERT_FIELD_EQ(&E_pthread_rwlock_unlock, ret);
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
test_pthread_rwlock_rdlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_rdlock, sizeof(struct pthread_rwlock_rdlock_event));
    /* call pthread_rwlock_rdlock with arguments */
    enable(fake_pthread_rwlock_rdlock);
     int  ret =                                   //
                                 pthread_rwlock_rdlock(                                    //
                                     E_pthread_rwlock_rdlock.lock                                  );
 ensure(ret == E_pthread_rwlock_rdlock.ret);
    disable();
}
static void
test_pthread_rwlock_tryrdlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_tryrdlock, sizeof(struct pthread_rwlock_tryrdlock_event));
    /* call pthread_rwlock_tryrdlock with arguments */
    enable(fake_pthread_rwlock_tryrdlock);
     int  ret =                                   //
                                 pthread_rwlock_tryrdlock(                                    //
                                     E_pthread_rwlock_tryrdlock.lock                                  );
 ensure(ret == E_pthread_rwlock_tryrdlock.ret);
    disable();
}
static void
test_pthread_rwlock_timedrdlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_timedrdlock, sizeof(struct pthread_rwlock_timedrdlock_event));
    /* call pthread_rwlock_timedrdlock with arguments */
    enable(fake_pthread_rwlock_timedrdlock);
     int  ret =                                   //
                                 pthread_rwlock_timedrdlock(                                    //
                                     E_pthread_rwlock_timedrdlock.lock,                           //
                                     E_pthread_rwlock_timedrdlock.abstime                                  );
 ensure(ret == E_pthread_rwlock_timedrdlock.ret);
    disable();
}
static void
test_pthread_rwlock_wrlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_wrlock, sizeof(struct pthread_rwlock_wrlock_event));
    /* call pthread_rwlock_wrlock with arguments */
    enable(fake_pthread_rwlock_wrlock);
     int  ret =                                   //
                                 pthread_rwlock_wrlock(                                    //
                                     E_pthread_rwlock_wrlock.lock                                  );
 ensure(ret == E_pthread_rwlock_wrlock.ret);
    disable();
}
static void
test_pthread_rwlock_trywrlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_trywrlock, sizeof(struct pthread_rwlock_trywrlock_event));
    /* call pthread_rwlock_trywrlock with arguments */
    enable(fake_pthread_rwlock_trywrlock);
     int  ret =                                   //
                                 pthread_rwlock_trywrlock(                                    //
                                     E_pthread_rwlock_trywrlock.lock                                  );
 ensure(ret == E_pthread_rwlock_trywrlock.ret);
    disable();
}
static void
test_pthread_rwlock_timedwrlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_timedwrlock, sizeof(struct pthread_rwlock_timedwrlock_event));
    /* call pthread_rwlock_timedwrlock with arguments */
    enable(fake_pthread_rwlock_timedwrlock);
     int  ret =                                   //
                                 pthread_rwlock_timedwrlock(                                    //
                                     E_pthread_rwlock_timedwrlock.lock,                           //
                                     E_pthread_rwlock_timedwrlock.abstime                                  );
 ensure(ret == E_pthread_rwlock_timedwrlock.ret);
    disable();
}
static void
test_pthread_rwlock_unlock(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_rwlock_unlock, sizeof(struct pthread_rwlock_unlock_event));
    /* call pthread_rwlock_unlock with arguments */
    enable(fake_pthread_rwlock_unlock);
     int  ret =                                   //
                                 pthread_rwlock_unlock(                                    //
                                     E_pthread_rwlock_unlock.lock                                  );
 ensure(ret == E_pthread_rwlock_unlock.ret);
    disable();
}

int
main()
{
    test_pthread_rwlock_rdlock();
    test_pthread_rwlock_tryrdlock();
    test_pthread_rwlock_timedrdlock();
    test_pthread_rwlock_wrlock();
    test_pthread_rwlock_trywrlock();
    test_pthread_rwlock_timedwrlock();
    test_pthread_rwlock_unlock();
    return 0;
}
