#define DICE_MODULE_SLOT SLOT_FIRST
#include "dice.h"

PS_SUBSCRIBE(CHAIN_TEST, EVENT_TEST, {
    int *n = event;
    if (*n == -1)
        return PS_STOP_CHAIN;
    *n += 1;
    return PS_HANDLER_OFF;
})
PS_SUBSCRIBE(CHAIN_TEST, ANY_EVENT, { *(int *)event += 10; })
PS_SUBSCRIBE_SLOT(CHAIN_TEST, ANY_EVENT, SLOT_LATER, { *(int *)event += 100; })
