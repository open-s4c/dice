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
    log_printf("Running fake");
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

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof((E)->field)) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMCPY, {
    struct memcpy_event *ev = EVENT_PAYLOAD(ev);
    log_printf("&ev->dest %p\n", &ev->dest);
    log_printf("&(&E_memcpy)->dest %p\n", &(&E_memcpy)->dest);
    log_printf("sizeof((E)->field) %lu\n", sizeof((&E_memcpy)->dest));
    log_printf("memcmp(&ev->dest, &(&E_memcpy)->dest, sizeof((&E_memcpy)->dest)) == 0 %d\n", memcmp(&ev->dest, &(&E_memcpy)->dest, sizeof((&E_memcpy)->dest)) == 0);
    ASSERT_FIELD_EQ(&E_memcpy, dest);
    ASSERT_FIELD_EQ(&E_memcpy, src);
    ASSERT_FIELD_EQ(&E_memcpy, num);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMCPY, {
    struct memcpy_event *ev = EVENT_PAYLOAD(ev);
    log_printf("&ev->dest %p\n", &ev->dest);
    log_printf("&(&E_memcpy)->dest %p\n", &(&E_memcpy)->dest);
    log_printf("sizeof((E)->field) %lu\n", sizeof((&E_memcpy)->dest));
    log_printf("memcmp(&ev->dest, &(&E_memcpy)->dest, sizeof((&E_memcpy)->dest)) == 0 %d\n", memcmp(&ev->dest, &(&E_memcpy)->dest, sizeof((&E_memcpy)->dest)) == 0);
    ASSERT_FIELD_EQ(&E_memcpy, dest);
    ASSERT_FIELD_EQ(&E_memcpy, src);
    ASSERT_FIELD_EQ(&E_memcpy, num);
 //ASSERT_FIELD_EQ(&E_memcpy, ret);
 ensure(E_memcpy.dest == E_memcpy.ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMMOVE, {
    struct memmove_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memmove, dest);
    ASSERT_FIELD_EQ(&E_memmove, src);
    ASSERT_FIELD_EQ(&E_memmove, count);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMMOVE, {
    struct memmove_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memmove, dest);
    ASSERT_FIELD_EQ(&E_memmove, src);
    ASSERT_FIELD_EQ(&E_memmove, count);
 //ASSERT_FIELD_EQ(&E_memmove, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MEMSET, {
    struct memset_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memset, ptr);
    ASSERT_FIELD_EQ(&E_memset, value);
    ASSERT_FIELD_EQ(&E_memset, num);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MEMSET, {
    struct memset_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_memset, ptr);
    ASSERT_FIELD_EQ(&E_memset, value);
    ASSERT_FIELD_EQ(&E_memset, num);
    //ASSERT_FIELD_EQ(&E_memset, ret);
})

/* test cases */
const int SIZE = 10;
static void
test_memcpy(void)
{
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
    log_printf("ret %p\n", ret); 
    log_printf("E_memcpy.dest %p\n", E_memcpy.dest);                                
    ensure(ret == E_memcpy.dest);
    ensure(strcmp(E_memcpy.dest, E_memcpy.src) == 0);
}
static void
test_memmove(void)
{
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
    ensure(strncmp((char *)E_memmove.dest, "Hi", 2) == 0);
}
static void
test_memset(void)
{
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
}

int
main()
{
    test_memcpy();
    test_memmove();
    test_memset();
    return 0;
}
