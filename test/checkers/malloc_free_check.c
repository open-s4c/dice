/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
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
            struct free_event *ev = EVENT_PAYLOAD(ev);

            // ignore frees of NULL pointers
            if (ev->ptr != NULL)
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

    /* Ensure the self module has been loaded. The self module interrupts
     * the INTERCEPT chains, handles TLS and redirects the events to
     * equivalent CAPTURE chains. */
    assert(intercepted[EVENT_MALLOC] == 0);
    assert(intercepted[EVENT_FREE] == 0);

    /* Ensure that at least one malloc was captured */
    assert(captured[EVENT_MALLOC] > 0);

    /* Ensure that all captured mallocs had a corresponding free */
    assert(captured[EVENT_MALLOC] == captured[EVENT_FREE]);
})
