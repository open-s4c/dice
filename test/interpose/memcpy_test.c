/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DICE_TEST_INTERPOSE
#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>
#include <dice/events/memcpy.h>

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
 * struct memcpy_event {
 *     void *dest;
 *     const void *src;
 *     size_t num;
 *      void *  ret;
 * };
 */
struct memcpy_event E_memcpy;
/* Expects struct to match this:
 *
 * struct memmove_event {
 *     void *dest;
 *     const void *src;
 *     size_t count;
 *      void *  ret;
 * };
 */
struct memmove_event E_memmove;
/* Expects struct to match this:
 *
 * struct memset_event {
 *     void *ptr;
 *     int value;
 *     size_t num;
 *      void *  ret;
 * };
 */
struct memset_event E_memset;

/* mock implementation of functions */
void *
fake_memcpy(void *dest ,const void *src ,size_t num)
{
    /* check that every argument is as expected */
    ensure(dest == E_memcpy.dest);
    ensure(src == E_memcpy.src);
    ensure(num == E_memcpy.num);
    /* return expected value */
 return E_memcpy.ret;
}
void *
fake_memmove(void *dest ,const void *src ,size_t count)
{
    /* check that every argument is as expected */
    ensure(dest == E_memmove.dest);
    ensure(src == E_memmove.src);
    ensure(count == E_memmove.count);
    /* return expected value */
 return E_memmove.ret;
}
void *
fake_memset(void *ptr, int value, size_t num)
{
    /* check that every argument is as expected */
    ensure(ptr == E_memset.ptr);
    ensure(value == E_memset.value);
    ensure(num == E_memset.num);
    /* return expected value */
 return E_memset.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMCPY, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memcpy_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memcpy, dest);
    ASSERT_FIELD_EQ(&E_memcpy, src);
    ASSERT_FIELD_EQ(&E_memcpy, num);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMCPY, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memcpy_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memcpy, dest);
    ASSERT_FIELD_EQ(&E_memcpy, src);
    ASSERT_FIELD_EQ(&E_memcpy, num);
 ASSERT_FIELD_EQ(&E_memcpy, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMMOVE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memmove_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memmove, dest);
    ASSERT_FIELD_EQ(&E_memmove, src);
    ASSERT_FIELD_EQ(&E_memmove, count);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMMOVE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memmove_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memmove, dest);
    ASSERT_FIELD_EQ(&E_memmove, src);
    ASSERT_FIELD_EQ(&E_memmove, count);
 ASSERT_FIELD_EQ(&E_memmove, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMSET, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memset_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memset, ptr);
    ASSERT_FIELD_EQ(&E_memset, value);
    ASSERT_FIELD_EQ(&E_memset, num);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMSET, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct memset_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memset, ptr);
    ASSERT_FIELD_EQ(&E_memset, value);
    ASSERT_FIELD_EQ(&E_memset, num);
 ASSERT_FIELD_EQ(&E_memset, ret);
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
test_memcpy(void)
{
    /* initialize event with random content */
    event_init(&E_memcpy, sizeof(struct memcpy_event));
    /* call memcpy with arguments */
    enable(fake_memcpy);
     void *  ret =                                   //
                                 memcpy(                                    //
                                     E_memcpy.dest,                           //
                                     E_memcpy.src,                           //
                                     E_memcpy.num                                  );
 ensure(ret == E_memcpy.ret);
    disable();
}
static void
test_memmove(void)
{
    /* initialize event with random content */
    event_init(&E_memmove, sizeof(struct memmove_event));
    /* call memmove with arguments */
    enable(fake_memmove);
     void *  ret =                                   //
                                 memmove(                                    //
                                     E_memmove.dest,                           //
                                     E_memmove.src,                           //
                                     E_memmove.count                                  );
 ensure(ret == E_memmove.ret);
    disable();
}
static void
test_memset(void)
{
    /* initialize event with random content */
    event_init(&E_memset, sizeof(struct memset_event));
    /* call memset with arguments */
    enable(fake_memset);
     void *  ret =                                   //
                                 memset(                                    //
                                     E_memset.ptr,                           //
                                     E_memset.value,                           //
                                     E_memset.num                                  );
 ensure(ret == E_memset.ret);
    disable();
}

int
main()
{
    test_memcpy();
    test_memmove();
    test_memset();
    return 0;
}
