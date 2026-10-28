#define DICE_MODULE_SLOT SLOT_SOURCE
#include "bench.h"
#include <dice/roll.h>

/* Keep a stable exported boundary for call-path inspection under LTO. */
__attribute__((noinline, used)) void
roll_publish(struct bench_event *event)
{
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_TEST, event, NULL);
}

__attribute__((noinline, used)) void
roll_publish_dynamic(struct bench_event *event)
{
    volatile chain_id chain = INTERCEPT_EVENT;
    PS_PUBLISH(chain, EVENT_TEST, event, NULL);
}
