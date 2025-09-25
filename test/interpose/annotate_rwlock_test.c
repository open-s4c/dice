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
#include <dice/events/rw_locks.h>

#define ensure(COND)                                                           \
    {                                                                          \
        if (!(COND)) {                                                         \
            log_fatal("error: %s", #COND);                                     \
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
 * struct AnnotateRWLockCreate_event {
 *     const char *file;
 *     int line;
 *     const volatile void *lock;
 * };
 */
struct AnnotateRWLockCreate_event E_AnnotateRWLockCreate;
/* Expects struct to match this:
 *
 * struct AnnotateRWLockDestroy_event {
 *     const char *file;
 *     int line;
 *     const volatile void *lock;
 * };
 */
struct AnnotateRWLockDestroy_event E_AnnotateRWLockDestroy;
/* Expects struct to match this:
 *
 * struct AnnotateRWLockAcquired_event {
 *     const char *file;
 *     int line;
 *     const volatile void *lock;
 *     long is_w;
 * };
 */
struct AnnotateRWLockAcquired_event E_AnnotateRWLockAcquired;
/* Expects struct to match this:
 *
 * struct AnnotateRWLockReleased_event {
 *     const char *file;
 *     int line;
 *     const volatile void *lock;
 *     long is_w;
 * };
 */
struct AnnotateRWLockReleased_event E_AnnotateRWLockReleased;

/* mock implementation of functions */
void
fake_AnnotateRWLockCreate(const char *file, int line, const volatile void *lock)
{
    /* check that every argument is as expected */
    ensure(file == E_AnnotateRWLockCreate.file);
    ensure(line == E_AnnotateRWLockCreate.line);
    ensure(lock == E_AnnotateRWLockCreate.lock);
    /* return expected value */
}
void
fake_AnnotateRWLockDestroy(const char *file, int line, const volatile void *lock)
{
    /* check that every argument is as expected */
    ensure(file == E_AnnotateRWLockDestroy.file);
    ensure(line == E_AnnotateRWLockDestroy.line);
    ensure(lock == E_AnnotateRWLockDestroy.lock);
    /* return expected value */
}
void
fake_AnnotateRWLockAcquired(const char *file, int line, const volatile void *lock , long is_w)
{
    /* check that every argument is as expected */
    ensure(file == E_AnnotateRWLockAcquired.file);
    ensure(line == E_AnnotateRWLockAcquired.line);
    ensure(lock == E_AnnotateRWLockAcquired.lock);
    ensure(is_w == E_AnnotateRWLockAcquired.is_w);
    /* return expected value */
}
void
fake_AnnotateRWLockReleased(const char *file, int line, const volatile void *lock , long is_w)
{
    /* check that every argument is as expected */
    ensure(file == E_AnnotateRWLockReleased.file);
    ensure(line == E_AnnotateRWLockReleased.line);
    ensure(lock == E_AnnotateRWLockReleased.lock);
    ensure(is_w == E_AnnotateRWLockReleased.is_w);
    /* return expected value */
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_ANNOTATERWLOCKCREATE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockCreate_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_ANNOTATERWLOCKCREATE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockCreate_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockCreate, lock);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_ANNOTATERWLOCKDESTROY, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockDestroy_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, lock);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_ANNOTATERWLOCKDESTROY, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockDestroy_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockDestroy, lock);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_ANNOTATERWLOCKACQUIRED, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockAcquired_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, lock);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, is_w);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_ANNOTATERWLOCKACQUIRED, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockAcquired_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, lock);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockAcquired, is_w);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_ANNOTATERWLOCKRELEASED, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockReleased_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, lock);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, is_w);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_ANNOTATERWLOCKRELEASED, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct AnnotateRWLockReleased_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, file);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, line);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, lock);
    ASSERT_FIELD_EQ(&E_AnnotateRWLockReleased, is_w);
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
test_AnnotateRWLockCreate(void)
{
    /* initialize event with random content */
    event_init(&E_AnnotateRWLockCreate, sizeof(struct AnnotateRWLockCreate_event));
    /* call AnnotateRWLockCreate with arguments */
    enable(fake_AnnotateRWLockCreate);
                                 AnnotateRWLockCreate(                                    //
                                     E_AnnotateRWLockCreate.file,                           //
                                     E_AnnotateRWLockCreate.line,                           //
                                     E_AnnotateRWLockCreate.lock                                  );
    disable();
}
static void
test_AnnotateRWLockDestroy(void)
{
    /* initialize event with random content */
    event_init(&E_AnnotateRWLockDestroy, sizeof(struct AnnotateRWLockDestroy_event));
    /* call AnnotateRWLockDestroy with arguments */
    enable(fake_AnnotateRWLockDestroy);
                                 AnnotateRWLockDestroy(                                    //
                                     E_AnnotateRWLockDestroy.file,                           //
                                     E_AnnotateRWLockDestroy.line,                           //
                                     E_AnnotateRWLockDestroy.lock                                  );
    disable();
}
static void
test_AnnotateRWLockAcquired(void)
{
    /* initialize event with random content */
    event_init(&E_AnnotateRWLockAcquired, sizeof(struct AnnotateRWLockAcquired_event));
    /* call AnnotateRWLockAcquired with arguments */
    enable(fake_AnnotateRWLockAcquired);
                                 AnnotateRWLockAcquired(                                    //
                                     E_AnnotateRWLockAcquired.file,                           //
                                     E_AnnotateRWLockAcquired.line,                           //
                                     E_AnnotateRWLockAcquired.lock,                           //
                                     E_AnnotateRWLockAcquired.is_w                                  );
    disable();
}
static void
test_AnnotateRWLockReleased(void)
{
    /* initialize event with random content */
    event_init(&E_AnnotateRWLockReleased, sizeof(struct AnnotateRWLockReleased_event));
    /* call AnnotateRWLockReleased with arguments */
    enable(fake_AnnotateRWLockReleased);
                                 AnnotateRWLockReleased(                                    //
                                     E_AnnotateRWLockReleased.file,                           //
                                     E_AnnotateRWLockReleased.line,                           //
                                     E_AnnotateRWLockReleased.lock,                           //
                                     E_AnnotateRWLockReleased.is_w                                  );
    disable();
}

int
main()
{
    test_AnnotateRWLockCreate();
    test_AnnotateRWLockDestroy();
    test_AnnotateRWLockAcquired();
    test_AnnotateRWLockReleased();
    return 0;
}
