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
#include <dice/events/mman.h>

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
 * struct mmap_event {
 *     void *addr;
 *     size_t length;
 *     int prot;
 *     int flags;
 *     int fd;
 *     off_t offset;
 *      void *  ret;
 * };
 */
struct mmap_event E_mmap;
/* Expects struct to match this:
 *
 * struct munmap_event {
 *     void *addr;
 *     size_t length;
 *      int  ret;
 * };
 */
struct munmap_event E_munmap;

/* mock implementation of functions */
void *
fake_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    /* check that every argument is as expected */
    ensure(addr == E_mmap.addr);
    ensure(length == E_mmap.length);
    ensure(prot == E_mmap.prot);
    ensure(flags == E_mmap.flags);
    ensure(fd == E_mmap.fd);
    ensure(offset == E_mmap.offset);
    /* return expected value */
 return E_mmap.ret;
}
int
fake_munmap(void *addr, size_t length)
{
    /* check that every argument is as expected */
    ensure(addr == E_munmap.addr);
    ensure(length == E_munmap.length);
    /* return expected value */
 return E_munmap.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MMAP, {
    if (!enabled())
        return PS_CB_STOP;
    struct mmap_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_mmap, addr);
    ASSERT_FIELD_EQ(&E_mmap, length);
    ASSERT_FIELD_EQ(&E_mmap, prot);
    ASSERT_FIELD_EQ(&E_mmap, flags);
    ASSERT_FIELD_EQ(&E_mmap, fd);
    ASSERT_FIELD_EQ(&E_mmap, offset);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MMAP, {
    if (!enabled())
        return PS_CB_STOP;
    struct mmap_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_mmap, addr);
    ASSERT_FIELD_EQ(&E_mmap, length);
    ASSERT_FIELD_EQ(&E_mmap, prot);
    ASSERT_FIELD_EQ(&E_mmap, flags);
    ASSERT_FIELD_EQ(&E_mmap, fd);
    ASSERT_FIELD_EQ(&E_mmap, offset);
 ASSERT_FIELD_EQ(&E_mmap, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MUNMAP, {
    if (!enabled())
        return PS_CB_STOP;
    struct munmap_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_munmap, addr);
    ASSERT_FIELD_EQ(&E_munmap, length);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MUNMAP, {
    if (!enabled())
        return PS_CB_STOP;
    struct munmap_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_munmap, addr);
    ASSERT_FIELD_EQ(&E_munmap, length);
 ASSERT_FIELD_EQ(&E_munmap, ret);
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
test_mmap(void)
{
    /* initialize event with random content */
    event_init(&E_mmap, sizeof(struct mmap_event));
    /* call mmap with arguments */
    enable(fake_mmap);
     void *  ret =                                   //
                                 mmap(                                    //
                                     E_mmap.addr,                           //
                                     E_mmap.length,                           //
                                     E_mmap.prot,                           //
                                     E_mmap.flags,                           //
                                     E_mmap.fd,                           //
                                     E_mmap.offset                                  );
 ensure(ret == E_mmap.ret);
    disable();
}
static void
test_munmap(void)
{
    /* initialize event with random content */
    event_init(&E_munmap, sizeof(struct munmap_event));
    /* call munmap with arguments */
    enable(fake_munmap);
     int  ret =                                   //
                                 munmap(                                    //
                                     E_munmap.addr,                           //
                                     E_munmap.length                                  );
 ensure(ret == E_munmap.ret);
    disable();
}

int
main()
{
    test_mmap();
    test_munmap();
    return 0;
}
