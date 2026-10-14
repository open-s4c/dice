#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "dice.h"

static int allocations, starts, exits;

int
configured_slot(void)
{
    return SLOT_OBSERVER;
}
int
configured_event(void)
{
    return EVENT_USER;
}
int
observed_allocations(void)
{
    return allocations;
}
int
observed_starts(void)
{
    return starts;
}
int
observed_exits(void)
{
    return exits;
}

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {
    struct malloc_event *allocation = event;
    if (allocation->size == 1234)
        allocations++;
})
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_START, { starts++; })
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_EXIT, { exits++; })
