#define DICE_MODULE_SLOT SLOT_DIRECT
#include "trace.h"

#if !DICE_PLUGINS
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_EARLY, {
    if (record(event, 'A') == PS_STOP_CHAIN)
        return PS_STOP_CHAIN;
    return PS_HANDLER_OFF;
})
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_EARLY,
                  { return record(event, 'a'); })
    #ifndef OMIT_MIDDLE
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_MIDDLE,
                  { return record(event, 'B'); })
    #endif
#endif

PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, { return record(event, 'D'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_LAST, {
    if (record(event, 'L') == PS_STOP_CHAIN)
        return PS_STOP_CHAIN;
    return PS_HANDLER_OFF;
})
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_LAST,
                  { return record(event, 'W'); })
