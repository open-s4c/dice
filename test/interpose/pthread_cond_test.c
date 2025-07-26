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
 * struct pthread_cond_wait_event {
 *     pthread_cond_t *cond;
 *     pthread_mutex_t *mutex;
 *      int  ret;
 * };
 */
struct pthread_cond_wait_event E_pthread_cond_wait;
/* Expects struct to match this:
 *
 * struct pthread_cond_timedwait_event {
 *     pthread_cond_t *cond;
 *     pthread_mutex_t *mutex;
 *     const struct timespec *abstime;
 *      int  ret;
 * };
 */
struct pthread_cond_timedwait_event E_pthread_cond_timedwait;
/* Expects struct to match this:
 *
 * struct pthread_cond_signal_event {
 *     pthread_cond_t *cond;
 *      int  ret;
 * };
 */
struct pthread_cond_signal_event E_pthread_cond_signal;
/* Expects struct to match this:
 *
 * struct pthread_cond_broadcast_event {
 *     pthread_cond_t *cond;
 *      int  ret;
 * };
 */
struct pthread_cond_broadcast_event E_pthread_cond_broadcast;

/* mock implementation of functions */
int
fake_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    /* check that every argument is as expected */
    ensure(cond == E_pthread_cond_wait.cond);
    ensure(mutex == E_pthread_cond_wait.mutex);
    /* return expected value */
 return E_pthread_cond_wait.ret;
}
int
fake_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    /* check that every argument is as expected */
    ensure(cond == E_pthread_cond_timedwait.cond);
    ensure(mutex == E_pthread_cond_timedwait.mutex);
    ensure(abstime == E_pthread_cond_timedwait.abstime);
    /* return expected value */
 return E_pthread_cond_timedwait.ret;
}
int
fake_pthread_cond_signal(pthread_cond_t *cond)
{
    /* check that every argument is as expected */
    ensure(cond == E_pthread_cond_signal.cond);
    /* return expected value */
 return E_pthread_cond_signal.ret;
}
int
fake_pthread_cond_broadcast(pthread_cond_t *cond)
{
    /* check that every argument is as expected */
    ensure(cond == E_pthread_cond_broadcast.cond);
    /* return expected value */
 return E_pthread_cond_broadcast.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_COND_WAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_wait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_wait, cond);
    ASSERT_FIELD_EQ(&E_pthread_cond_wait, mutex);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_COND_WAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_wait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_wait, cond);
    ASSERT_FIELD_EQ(&E_pthread_cond_wait, mutex);
 ASSERT_FIELD_EQ(&E_pthread_cond_wait, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_COND_TIMEDWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_timedwait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, cond);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, mutex);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, abstime);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_COND_TIMEDWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_timedwait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, cond);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, mutex);
    ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, abstime);
 ASSERT_FIELD_EQ(&E_pthread_cond_timedwait, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_COND_SIGNAL, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_signal_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_signal, cond);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_COND_SIGNAL, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_signal_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_signal, cond);
 ASSERT_FIELD_EQ(&E_pthread_cond_signal, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_COND_BROADCAST, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_broadcast_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_broadcast, cond);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_COND_BROADCAST, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_cond_broadcast_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_cond_broadcast, cond);
 ASSERT_FIELD_EQ(&E_pthread_cond_broadcast, ret);
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
test_pthread_cond_wait(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_cond_wait, sizeof(struct pthread_cond_wait_event));
    /* call pthread_cond_wait with arguments */
    enable(fake_pthread_cond_wait);
     int  ret =                                   //
                                 pthread_cond_wait(                                    //
                                     E_pthread_cond_wait.cond,                           //
                                     E_pthread_cond_wait.mutex                                  );
 ensure(ret == E_pthread_cond_wait.ret);
    disable();
}
static void
test_pthread_cond_timedwait(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_cond_timedwait, sizeof(struct pthread_cond_timedwait_event));
    /* call pthread_cond_timedwait with arguments */
    enable(fake_pthread_cond_timedwait);
     int  ret =                                   //
                                 pthread_cond_timedwait(                                    //
                                     E_pthread_cond_timedwait.cond,                           //
                                     E_pthread_cond_timedwait.mutex,                           //
                                     E_pthread_cond_timedwait.abstime                                  );
 ensure(ret == E_pthread_cond_timedwait.ret);
    disable();
}
static void
test_pthread_cond_signal(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_cond_signal, sizeof(struct pthread_cond_signal_event));
    /* call pthread_cond_signal with arguments */
    enable(fake_pthread_cond_signal);
     int  ret =                                   //
                                 pthread_cond_signal(                                    //
                                     E_pthread_cond_signal.cond                                  );
 ensure(ret == E_pthread_cond_signal.ret);
    disable();
}
static void
test_pthread_cond_broadcast(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_cond_broadcast, sizeof(struct pthread_cond_broadcast_event));
    /* call pthread_cond_broadcast with arguments */
    enable(fake_pthread_cond_broadcast);
     int  ret =                                   //
                                 pthread_cond_broadcast(                                    //
                                     E_pthread_cond_broadcast.cond                                  );
 ensure(ret == E_pthread_cond_broadcast.ret);
    disable();
}

int
main()
{
    test_pthread_cond_wait();
    test_pthread_cond_timedwait();
    test_pthread_cond_signal();
    test_pthread_cond_broadcast();
    return 0;
}
