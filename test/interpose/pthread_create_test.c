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
 * struct pthread_join_event {
 *     pthread_t thread;
 *     void **ptr;
 *      int  ret;
 * };
 */
struct pthread_join_event E_pthread_join;

/* mock implementation of functions */
int
fake_pthread_join(pthread_t thread, void **ptr)
{
    /* check that every argument is as expected */
    ensure(thread == E_pthread_join.thread);
    ensure(ptr == E_pthread_join.ptr);
    /* return expected value */
 return E_pthread_join.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_PTHREAD_JOIN, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct pthread_join_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_pthread_join, thread);
    ASSERT_FIELD_EQ(&E_pthread_join, ptr);
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
test_pthread_join(void)
{
    /* initialize event with random content */
    event_init(&E_pthread_join, sizeof(struct pthread_join_event));
    /* call pthread_join with arguments */
    enable(fake_pthread_join);
     int  ret =                                   //
                                 pthread_join(                                    //
                                     E_pthread_join.thread,                           //
                                     E_pthread_join.ptr                                  );
 ensure(ret == E_pthread_join.ret);
    disable();
}

int
main()
{
    test_pthread_join();
    return 0;
}
