#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "dice.h"

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {
    struct malloc_event *allocation = event;
    if (allocation->size == 1234)
        puts("linked observer: malloc(1234)");
    return PS_OK;
})

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_START,
             { puts("linked observer: thread start"); })
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_EXIT,
             { puts("linked observer: thread exit"); })
