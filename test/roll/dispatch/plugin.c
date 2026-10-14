#define DICE_PLUGIN_MODULE
#include "trace.h"

PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_EARLY, {
    if (record(event, 'A') == PS_STOP_CHAIN)
        return PS_STOP_CHAIN;
    return PS_HANDLER_OFF;
})
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_EARLY,
                  { return record(event, 'a'); })
/* Register even when a strong linked definition replaces this fallback. */
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_DIRECT,
                  { return record(event, 'x'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_MIDDLE,
                  { return record(event, 'B'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_LAST,
                  { return record(event, 'y'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_LAST,
                  { return record(event, 'z'); })

#if DICE_PLUGINS
/* Register out of order: the trailing phase must still run in slot order. */
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, 14, { return record(event, 'U'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, 11, {
    PS_PUBLISH(CHAIN_OTHER, EVENT_OTHER, event, md);
    return record(event, 'T');
})
PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, { return record(event, 'V'); })
#endif
