#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "bench.h"
#include <dice/roll.h>

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_TEST, {
    struct bench_event *ev = event;
    if (self_id(md) == NO_THREAD)
        return PS_ERROR;
    ev->value++;
    /* Retain the per-event work even when every handler folds under LTO. */
    __asm__ volatile("" : "+m"(ev->value) : : "memory");
})
