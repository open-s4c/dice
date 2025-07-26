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
#include <dice/events/semaphore.h>

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
 * struct sem_post_event {
 *     sem_t *sem;
 *      int  ret;
 * };
 */
struct sem_post_event E_sem_post;
/* Expects struct to match this:
 *
 * struct sem_wait_event {
 *     sem_t *sem;
 *      int  ret;
 * };
 */
struct sem_wait_event E_sem_wait;
/* Expects struct to match this:
 *
 * struct sem_trywait_event {
 *     sem_t *sem;
 *      int  ret;
 * };
 */
struct sem_trywait_event E_sem_trywait;
/* Expects struct to match this:
 *
 * struct sem_timedwait_event {
 *     sem_t *sem;
 *     const struct timespec *abstime;
 *      int  ret;
 * };
 */
struct sem_timedwait_event E_sem_timedwait;

/* mock implementation of functions */
int
fake_sem_post(sem_t *sem)
{
    /* check that every argument is as expected */
    ensure(sem == E_sem_post.sem);
    /* return expected value */
 return E_sem_post.ret;
}
int
fake_sem_wait(sem_t *sem)
{
    /* check that every argument is as expected */
    ensure(sem == E_sem_wait.sem);
    /* return expected value */
 return E_sem_wait.ret;
}
int
fake_sem_trywait(sem_t *sem)
{
    /* check that every argument is as expected */
    ensure(sem == E_sem_trywait.sem);
    /* return expected value */
 return E_sem_trywait.ret;
}
int
fake_sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    /* check that every argument is as expected */
    ensure(sem == E_sem_timedwait.sem);
    ensure(abstime == E_sem_timedwait.abstime);
    /* return expected value */
 return E_sem_timedwait.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_SEM_POST, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_post_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_post, sem);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_SEM_POST, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_post_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_post, sem);
 ASSERT_FIELD_EQ(&E_sem_post, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_SEM_WAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_wait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_wait, sem);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_SEM_WAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_wait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_wait, sem);
 ASSERT_FIELD_EQ(&E_sem_wait, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_SEM_TRYWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_trywait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_trywait, sem);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_SEM_TRYWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_trywait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_trywait, sem);
 ASSERT_FIELD_EQ(&E_sem_trywait, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_SEM_TIMEDWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_timedwait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_timedwait, sem);
    ASSERT_FIELD_EQ(&E_sem_timedwait, abstime);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_SEM_TIMEDWAIT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct sem_timedwait_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_sem_timedwait, sem);
    ASSERT_FIELD_EQ(&E_sem_timedwait, abstime);
 ASSERT_FIELD_EQ(&E_sem_timedwait, ret);
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
test_sem_post(void)
{
    /* initialize event with random content */
    event_init(&E_sem_post, sizeof(struct sem_post_event));
    /* call sem_post with arguments */
    enable(fake_sem_post);
     int  ret =                                   //
                                 sem_post(                                    //
                                     E_sem_post.sem                                  );
 ensure(ret == E_sem_post.ret);
    disable();
}
static void
test_sem_wait(void)
{
    /* initialize event with random content */
    event_init(&E_sem_wait, sizeof(struct sem_wait_event));
    /* call sem_wait with arguments */
    enable(fake_sem_wait);
     int  ret =                                   //
                                 sem_wait(                                    //
                                     E_sem_wait.sem                                  );
 ensure(ret == E_sem_wait.ret);
    disable();
}
static void
test_sem_trywait(void)
{
    /* initialize event with random content */
    event_init(&E_sem_trywait, sizeof(struct sem_trywait_event));
    /* call sem_trywait with arguments */
    enable(fake_sem_trywait);
     int  ret =                                   //
                                 sem_trywait(                                    //
                                     E_sem_trywait.sem                                  );
 ensure(ret == E_sem_trywait.ret);
    disable();
}
static void
test_sem_timedwait(void)
{
    /* initialize event with random content */
    event_init(&E_sem_timedwait, sizeof(struct sem_timedwait_event));
    /* call sem_timedwait with arguments */
    enable(fake_sem_timedwait);
     int  ret =                                   //
                                 sem_timedwait(                                    //
                                     E_sem_timedwait.sem,                           //
                                     E_sem_timedwait.abstime                                  );
 ensure(ret == E_sem_timedwait.ret);
    disable();
}

int
main()
{
    test_sem_post();
    test_sem_wait();
    test_sem_trywait();
    test_sem_timedwait();
    return 0;
}
