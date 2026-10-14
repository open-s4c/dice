#define DICE_PLUGIN_MODULE
#define DICE_MODULE_SLOT SLOT_OBSERVER
#include "dice.h"

PS_SUBSCRIBE(CHAIN_OUTPUT, EVENT_TEST, { *(int *)event += 100; })
PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, {})
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_START, {})
PS_SUBSCRIBE_SLOT(CHAIN_WILD, ANY_EVENT, SLOT_WILDCARD, {
    /* Dynamic event forwarding is covered by the produces wildcard. */
    PS_PUBLISH(CHAIN_OUTPUT, type, event, md);
})
