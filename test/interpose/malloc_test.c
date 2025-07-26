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
#include <dice/events/malloc.h>

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
 * struct malloc_event {
 *     size_t size;
 *      void *  ret;
 * };
 */
struct malloc_event E_malloc;
/* Expects struct to match this:
 *
 * struct calloc_event {
 *     size_t number;
 *     size_t size;
 *      void *  ret;
 * };
 */
struct calloc_event E_calloc;
/* Expects struct to match this:
 *
 * struct realloc_event {
 *     void *ptr;
 *     size_t size;
 *      void *  ret;
 * };
 */
struct realloc_event E_realloc;
/* Expects struct to match this:
 *
 * struct free_event {
 *     void *ptr;
 * };
 */
struct free_event E_free;
/* Expects struct to match this:
 *
 * struct posix_memalign_event {
 *     void **ptr;
 *     size_t alignment;
 *     size_t size;
 *      int  ret;
 * };
 */
struct posix_memalign_event E_posix_memalign;
/* Expects struct to match this:
 *
 * struct aligned_alloc_event {
 *     size_t alignment;
 *     size_t size;
 *      void *  ret;
 * };
 */
struct aligned_alloc_event E_aligned_alloc;

/* mock implementation of functions */
void *
fake_malloc(size_t size)
{
    /* check that every argument is as expected */
    ensure(size == E_malloc.size);
    /* return expected value */
 return E_malloc.ret;
}
void *
fake_calloc(size_t number, size_t size)
{
    /* check that every argument is as expected */
    ensure(number == E_calloc.number);
    ensure(size == E_calloc.size);
    /* return expected value */
 return E_calloc.ret;
}
void *
fake_realloc(void *ptr, size_t size)
{
    /* check that every argument is as expected */
    ensure(ptr == E_realloc.ptr);
    ensure(size == E_realloc.size);
    /* return expected value */
 return E_realloc.ret;
}
void
fake_free(void *ptr)
{
    /* check that every argument is as expected */
    ensure(ptr == E_free.ptr);
    /* return expected value */
}
int
fake_posix_memalign(void **ptr, size_t alignment, size_t size)
{
    /* check that every argument is as expected */
    ensure(ptr == E_posix_memalign.ptr);
    ensure(alignment == E_posix_memalign.alignment);
    ensure(size == E_posix_memalign.size);
    /* return expected value */
 return E_posix_memalign.ret;
}
void *
fake_aligned_alloc(size_t alignment, size_t size)
{
    /* check that every argument is as expected */
    ensure(alignment == E_aligned_alloc.alignment);
    ensure(size == E_aligned_alloc.size);
    /* return expected value */
 return E_aligned_alloc.ret;
}

#define ASSERT_FIELD_EQ(E, field)                                              \
    ensure(memcmp(&ev->field, &(E)->field, sizeof(__typeof((E)->field))) == 0);

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct malloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_malloc, size);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct malloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_malloc, size);
 ASSERT_FIELD_EQ(&E_malloc, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_CALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct calloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_calloc, number);
    ASSERT_FIELD_EQ(&E_calloc, size);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_CALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct calloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_calloc, number);
    ASSERT_FIELD_EQ(&E_calloc, size);
 ASSERT_FIELD_EQ(&E_calloc, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_REALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct realloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_realloc, ptr);
    ASSERT_FIELD_EQ(&E_realloc, size);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_REALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct realloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_realloc, ptr);
    ASSERT_FIELD_EQ(&E_realloc, size);
 ASSERT_FIELD_EQ(&E_realloc, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_FREE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct free_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_free, ptr);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_FREE, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct free_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_free, ptr);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_POSIX_MEMALIGN, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct posix_memalign_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_posix_memalign, ptr);
    ASSERT_FIELD_EQ(&E_posix_memalign, alignment);
    ASSERT_FIELD_EQ(&E_posix_memalign, size);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_POSIX_MEMALIGN, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct posix_memalign_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_posix_memalign, ptr);
    ASSERT_FIELD_EQ(&E_posix_memalign, alignment);
    ASSERT_FIELD_EQ(&E_posix_memalign, size);
 ASSERT_FIELD_EQ(&E_posix_memalign, ret);
})
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_ALIGNED_ALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct aligned_alloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_aligned_alloc, alignment);
    ASSERT_FIELD_EQ(&E_aligned_alloc, size);
})

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_ALIGNED_ALLOC, {
    if (!enabled())
        return PS_STOP_CHAIN;
    struct aligned_alloc_event *ev = EVENT_PAYLOAD(ev);
    ASSERT_FIELD_EQ(&E_aligned_alloc, alignment);
    ASSERT_FIELD_EQ(&E_aligned_alloc, size);
 ASSERT_FIELD_EQ(&E_aligned_alloc, ret);
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
test_malloc(void)
{
    /* initialize event with random content */
    event_init(&E_malloc, sizeof(struct malloc_event));
    /* call malloc with arguments */
    enable(fake_malloc);
     void *  ret =                                   //
                                 malloc(                                    //
                                     E_malloc.size                                  );
 ensure(ret == E_malloc.ret);
    disable();
}
static void
test_calloc(void)
{
    /* initialize event with random content */
    event_init(&E_calloc, sizeof(struct calloc_event));
    /* call calloc with arguments */
    enable(fake_calloc);
     void *  ret =                                   //
                                 calloc(                                    //
                                     E_calloc.number,                           //
                                     E_calloc.size                                  );
 ensure(ret == E_calloc.ret);
    disable();
}
static void
test_realloc(void)
{
    /* initialize event with random content */
    event_init(&E_realloc, sizeof(struct realloc_event));
    /* call realloc with arguments */
    enable(fake_realloc);
     void *  ret =                                   //
                                 realloc(                                    //
                                     E_realloc.ptr,                           //
                                     E_realloc.size                                  );
 ensure(ret == E_realloc.ret);
    disable();
}
static void
test_free(void)
{
    /* initialize event with random content */
    event_init(&E_free, sizeof(struct free_event));
    /* call free with arguments */
    enable(fake_free);
                                 free(                                    //
                                     E_free.ptr                                  );
    disable();
}
static void
test_posix_memalign(void)
{
    /* initialize event with random content */
    event_init(&E_posix_memalign, sizeof(struct posix_memalign_event));
    /* call posix_memalign with arguments */
    enable(fake_posix_memalign);
     int  ret =                                   //
                                 posix_memalign(                                    //
                                     E_posix_memalign.ptr,                           //
                                     E_posix_memalign.alignment,                           //
                                     E_posix_memalign.size                                  );
 ensure(ret == E_posix_memalign.ret);
    disable();
}
static void
test_aligned_alloc(void)
{
    /* initialize event with random content */
    event_init(&E_aligned_alloc, sizeof(struct aligned_alloc_event));
    /* call aligned_alloc with arguments */
    enable(fake_aligned_alloc);
     void *  ret =                                   //
                                 aligned_alloc(                                    //
                                     E_aligned_alloc.alignment,                           //
                                     E_aligned_alloc.size                                  );
 ensure(ret == E_aligned_alloc.ret);
    disable();
}

int
main()
{
    test_malloc();
    test_calloc();
    test_realloc();
    test_free();
    test_posix_memalign();
    test_aligned_alloc();
    return 0;
}
