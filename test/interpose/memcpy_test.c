/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stddef.h>
#include <stdlib.h>
#define memcpy fake_memcpy
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

/* test case */
const int SIZE = 10;

static void
test_memcpy(void)
{
    enable(fake_memcpy);
    char dest[SIZE];
    E_memcpy.dest = dest;
    char hello[] = "Hello!";
    E_memcpy.src= hello;
    E_memcpy.num = strlen(E_memcpy.src) + 1;
    E_memcpy.ret = E_memcpy.dest;
    void *  ret =                                   //
                                 memcpy(                                    //
                                     E_memcpy.dest,                           //
                                     E_memcpy.src,                           //
                                     E_memcpy.num                                  );                             
    ensure(ret == E_memcpy.dest);
    disable();
}

static void
test_memmove(void)
{
    enable(fake_memmove);
    char dest[SIZE];
    E_memmove.dest = dest;
    char hello[] = "Hi there!";
    E_memmove.src= hello;
    E_memmove.count = 2;
    E_memmove.ret = E_memmove.dest;
    void *  ret =                                   //
                                 memmove(                                    //
                                     E_memmove.dest,                           //
                                     E_memmove.src,                           //
                                     E_memmove.count                                  );
    ensure(ret == E_memmove.dest);
    disable();
}

static void
test_memset(void)
{
    enable(fake_memset);
    char dest[SIZE];
    E_memset.ptr = dest;
    E_memset.value= 3;
    E_memset.num = 5;
    E_memset.ret = E_memset.ptr;
    void *  ret =                                   //
                                 memset(                                    //
                                     E_memset.ptr,                           //
                                     E_memset.value,                           //
                                     E_memset.num                                  );
    ensure(ret == E_memset.ptr);
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
