/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifdef DICE_ROLLED
    #include <dice/roll.h>
#endif
#include <malloc.h>
#include <stdlib.h>

#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/events/malloc.h>
#include <dice/module.h>

/* Direct libc calls may carry a leaf attribute. State shared with their
 * subscribers must remain visible across those calls in this translation unit.
 */
static void *volatile expected;
static volatile unsigned before, after;
static volatile int replace;
static volatile size_t observed;

#if defined(__NetBSD__)
static size_t
replacement(const void *ptr)
#else
static size_t
replacement(void *ptr)
#endif
{
    ensure(ptr == expected);
    return 123;
}

PS_SUBSCRIBE(CHAIN_INTERCEPT_BEFORE, EVENT_MALLOC_USABLE_SIZE, {
    struct malloc_usable_size_event *ev = event;
    ensure(ev->ptr == expected);
    ensure(ev->pc != NULL);
    ensure(ev->func != NULL);
    ensure(ev->ret == 0);
    before++;
    if (replace)
        ev->func = replacement;
})

PS_SUBSCRIBE(CHAIN_INTERCEPT_AFTER, EVENT_MALLOC_USABLE_SIZE, {
    struct malloc_usable_size_event *ev = event;
    ensure(before == after + 1);
    observed = ev->ret;
    after++;
    if (replace)
        ev->ret = 456;
})

int
main(void)
{
    expected = malloc(42);
    ensure(expected != NULL);
    size_t size = malloc_usable_size(expected);
    ensure(size >= 42);
    ensure(observed == size);
    ensure(before == 1 && after == 1);
    replace = 1;
    ensure(malloc_usable_size(expected) == 456);
    ensure(observed == 123);
    ensure(before == 2 && after == 2);
    free(expected);
    return 0;
}
