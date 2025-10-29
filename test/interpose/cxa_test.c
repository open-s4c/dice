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
#include <dice/events/cxa.h>

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
 * struct __cxa_guard_acquire_event {
 *     void *addr;
 *      int  ret;
 * };
 */
struct __cxa_guard_acquire_event E___cxa_guard_acquire;
/* Expects struct to match this:
 *
 * struct __cxa_guard_release_event {
 *     void *addr;
 *      int  ret;
 * };
 */
struct __cxa_guard_release_event E___cxa_guard_release;
/* Expects struct to match this:
 *
 * struct __cxa_guard_abort_event {
 *     void *addr;
 * };
 */
struct __cxa_guard_abort_event E___cxa_guard_abort;

/* mock implementation of functions */
int
fake___cxa_guard_acquire(void *addr)
{
    /* check that every argument is as expected (unless should be skipped). */
    ensure(addr == E___cxa_guard_acquire.addr);

    /* skipped arguments should be void-cast to silent compiler warnings. */


    /* mark as called*/
    ensure(!called);
    called = true;

    /* return expected value */
 return E___cxa_guard_acquire.ret;
}
int
fake___cxa_guard_release(void *addr)
{
    /* check that every argument is as expected (unless should be skipped). */
    ensure(addr == E___cxa_guard_release.addr);

    /* skipped arguments should be void-cast to silent compiler warnings. */


    /* mark as called*/
    ensure(!called);
    called = true;

    /* return expected value */
 return E___cxa_guard_release.ret;
}
void
fake___cxa_guard_abort(void *addr)
{
    /* check that every argument is as expected (unless should be skipped). */
    ensure(addr == E___cxa_guard_abort.addr);

    /* skipped arguments should be void-cast to silent compiler warnings. */


    /* mark as called*/
    ensure(!called);
    called = true;

    /* return expected value */
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT___CXA_GUARD_ACQUIRE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_acquire_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_acquire, addr);

    // must be enabled.
    ensure(enabled());
    ev->func = fake___cxa_guard_acquire;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT___CXA_GUARD_ACQUIRE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_acquire_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_acquire, addr);
 ASSERT_FIELD_EQ(&E___cxa_guard_acquire, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT___CXA_GUARD_RELEASE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_release_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_release, addr);

    // must be enabled.
    ensure(enabled());
    ev->func = fake___cxa_guard_release;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT___CXA_GUARD_RELEASE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_release_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_release, addr);
 ASSERT_FIELD_EQ(&E___cxa_guard_release, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT___CXA_GUARD_ABORT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_abort_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_abort, addr);

    // must be enabled.
    ensure(enabled());
    ev->func = fake___cxa_guard_abort;
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT___CXA_GUARD_ABORT, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct __cxa_guard_abort_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E___cxa_guard_abort, addr);
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
test___cxa_guard_acquire(void)
{
    /* initialize event with random content */
    event_init(&E___cxa_guard_acquire, sizeof(struct __cxa_guard_acquire_event));

    /* ensure that fields that must be equal are actually equal */

    /* call __cxa_guard_acquire with arguments */
    enable(fake___cxa_guard_acquire);
     int  ret =                                   //
                                 __cxa_guard_acquire(                                    //
                                     E___cxa_guard_acquire.addr                                  );
    ensure(called);
 ensure(ret == E___cxa_guard_acquire.ret);
    disable();
}
static void
test___cxa_guard_release(void)
{
    /* initialize event with random content */
    event_init(&E___cxa_guard_release, sizeof(struct __cxa_guard_release_event));

    /* ensure that fields that must be equal are actually equal */

    /* call __cxa_guard_release with arguments */
    enable(fake___cxa_guard_release);
     int  ret =                                   //
                                 __cxa_guard_release(                                    //
                                     E___cxa_guard_release.addr                                  );
    ensure(called);
 ensure(ret == E___cxa_guard_release.ret);
    disable();
}
static void
test___cxa_guard_abort(void)
{
    /* initialize event with random content */
    event_init(&E___cxa_guard_abort, sizeof(struct __cxa_guard_abort_event));

    /* ensure that fields that must be equal are actually equal */

    /* call __cxa_guard_abort with arguments */
    enable(fake___cxa_guard_abort);
                                 __cxa_guard_abort(                                    //
                                     E___cxa_guard_abort.addr                                  );
    ensure(called);
    disable();
}

int
main()
{
    test___cxa_guard_acquire();
    test___cxa_guard_release();
    test___cxa_guard_abort();
    return 0;
}
