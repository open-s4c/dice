/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */

#include <assert.h>
#include <stdio.h>

#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/events/malloc.h>
#include <dice/module.h>
#include <dice/pubsub.h>

static int intercepted[] = {
    [EVENT_MALLOC] = 0,
    [EVENT_FREE]   = 0,
};
static int captured[] = {
    [EVENT_MALLOC] = 0,
    [EVENT_FREE]   = 0,
};

PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_TYPE, {
    switch (type) {
        case EVENT_MALLOC:
        case EVENT_CALLOC:
        case EVENT_ALIGNED_ALLOC:
        case EVENT_POSIX_MEMALIGN:
            captured[EVENT_MALLOC]++;
            break;
        case EVENT_FREE: {
            captured[EVENT_FREE]++;
            break;
        }
        default:
            break;
    }
})

#define p(a, x) log_printf("%11s[%s]   \t= %d\n", #a, #x, a[x])

DICE_MODULE_FINI({
    p(captured, EVENT_MALLOC);
    p(captured, EVENT_FREE);
    p(intercepted, EVENT_MALLOC);
    p(intercepted, EVENT_FREE);

    // Ensure the self module has been loaded. The self module interrupts
    // the INTERCEPT chains, handles TLS and redirects the events to
    // equivalent CAPTURE chains.
    assert(intercepted[EVENT_MALLOC] == 0);
    assert(intercepted[EVENT_FREE] == 0);

    // Ensure that at least one malloc was captured
    assert(captured[EVENT_MALLOC] > 0);

// We would like to ensure that all captured mallocs had a corresponding
// free. But that is quite hard. Free is also often called with 0 pointer.
// At the moment, on Linux the best to claim is that there are more frees
// than mallocs (or the same number). On other systems, the best claim is that
// frees are not 0.
#if defined(__linux__)
    assert(captured[EVENT_MALLOC] <= captured[EVENT_FREE]);
#else
    assert(captured[EVENT_MALLOC] > 0);
    assert(captured[EVENT_FREE] > 0);
#endif
})
