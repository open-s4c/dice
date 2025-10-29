/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>
#include <dice/events/pthread.h>

static void *symbol;
static bool called;
/* we need to declare this as noinline, otherwise the optimization of the
 * compiler gets rid of the symbol. */
static __attribute__((noinline)) void
enable(void *foo)
{
    symbol = foo;
    called = false;
}
static __attribute__((noinline)) void
disable(void)
{
    symbol = NULL;
    called = false;
}
static inline bool
enabled(void)
{
    return symbol != NULL;
}

/* Expects struct to match this:
 *
 * struct pthread_create_event {
 *     pthread_t *thread;
 *     const pthread_attr_t *attr;
 *     void *(*run;
 *      int  ret;
 * };
 */
struct pthread_create_event E_pthread_create;
/* Expects struct to match this:
 *
 * struct pthread_join_event {
 *     pthread_t thread;
 *     void **ptr;
 *      int  ret;
 * };
 */
struct pthread_join_event E_pthread_join;

/* mock implementation of functions */
int
fake_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*run)(void *), void *arg)
{
    /* check that every argument is as expected (unless should be skipped). */
    ensure(thread == E_pthread_create.thread);
    ensure(attr == E_pthread_create.attr);

    /* skipped arguments should be void-cast to silent compiler warnings. */
    (void)run;
    (void)arg;


    /* mark as called*/
    ensure(!called);
    called = true;

    /* return expected value */
 return E_pthread_create.ret;
}
int
fake_pthread_join(pthread_t thread, void **ptr)
{
    /* check that every argument is as expected (unless should be skipped). */
    ensure(thread == E_pthread_join.thread);
    ensure(ptr == E_pthread_join.ptr);

    /* skipped arguments should be void-cast to silent compiler warnings. */


    /* mark as called*/
    ensure(!called);
    called = true;

    /* return expected value */
 return E_pthread_join.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_CREATE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_create_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_create, thread);
    ASSERT_FIELD_EQ(&E_pthread_create, attr);

    // must be enabled.
    ensure(enabled());
    ev->func = fake_pthread_create;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_CREATE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_create_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_create, thread);
    ASSERT_FIELD_EQ(&E_pthread_create, attr);
 ASSERT_FIELD_EQ(&E_pthread_create, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_JOIN, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_join, thread);
    ASSERT_FIELD_EQ(&E_pthread_join, ptr);

    // must be enabled.
    ensure(enabled());
    ev->func = fake_pthread_join;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_PTHREAD_JOIN, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_join, thread);
    ASSERT_FIELD_EQ(&E_pthread_join, ptr);
 ASSERT_FIELD_EQ(&E_pthread_join, ret);
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
test_pthread_create(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_create, sizeof(struct pthread_create_event));

    /* ensure that fields that must be equal are actually equal */

    /* call pthread_create with arguments */
    enable(fake_pthread_create);
     int  ret =                                   //
                                 pthread_create(                                    //
                                     E_pthread_create.thread,                           //
                                     E_pthread_create.attr,                           //
                                     E_pthread_create.run,                           //
                                     E_pthread_create.arg                                  );
    ensure(called);
 ensure(ret == E_pthread_create.ret);
    disable();
}
static void
test_pthread_join(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_join, sizeof(struct pthread_join_event));

    /* ensure that fields that must be equal are actually equal */

    /* call pthread_join with arguments */
    enable(fake_pthread_join);
     int  ret =                                   //
                                 pthread_join(                                    //
                                     E_pthread_join.thread,                           //
                                     E_pthread_join.ptr                                  );
    ensure(called);
 ensure(ret == E_pthread_join.ret);
    disable();
}

int
main()
{
    test_pthread_create();
    test_pthread_join();
    return 0;
}
