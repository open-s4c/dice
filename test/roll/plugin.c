#include "dice.h"
/* An unconfigured plugin runs in the trailing callback phase. */
PS_SUBSCRIBE(CHAIN_TEST, EVENT_TEST, { *(int *)event += 1000; })
PS_SUBSCRIBE(CHAIN_EMPTY, EVENT_UNUSED, { *(int *)event += 2000; })
PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {
    struct malloc_event *ev = event;
    if (ev->size == 1234)
        puts("allocation captured");
})
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_START, { puts("thread captured"); })
