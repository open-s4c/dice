#define DICE_PLUGIN_MODULE
#include <assert.h>
#include <string.h>

#include "trace.h"

PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, SLOT_EARLY, {
    if (record(event, 'A') == PS_STOP_CHAIN)
        return PS_STOP_CHAIN;
    return PS_HANDLER_OFF;
})
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_EARLY,
                  { return record(event, 'a'); })
PS_HANDLER_DEF(CHAIN_FLOW, EVENT_TEST, SLOT_DIRECT,
               { return record(event, 'x'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_MIDDLE,
                  { return record(event, 'B'); })
PS_HANDLER_DEF(CHAIN_FLOW, EVENT_TEST, SLOT_LAST,
               { return record(event, 'y'); })
PS_HANDLER_DEF(CHAIN_FLOW, ANY_EVENT, SLOT_LAST, { return record(event, 'z'); })

static void DICE_CTOR
register_fallbacks(void)
{
    int expected = getenv("WITH_STRONG") ? PS_INVALID : PS_OK;
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST,
                        PS_HANDLER(CHAIN_FLOW, EVENT_TEST, SLOT_DIRECT),
                        SLOT_DIRECT) == expected);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST,
                        PS_HANDLER(CHAIN_FLOW, EVENT_TEST, SLOT_LAST),
                        SLOT_LAST) == expected);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT,
                        PS_HANDLER(CHAIN_FLOW, ANY_EVENT, SLOT_LAST),
                        SLOT_LAST) == expected);
}

#if DICE_CALLBACKS
/* Register out of order: the trailing phase must still run in slot order. */
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, 14, { return record(event, 'U'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_TEST, 11, {
    struct trace other = {0};
    PS_PUBLISH(CHAIN_OTHER, EVENT_OTHER, &other, md);
    assert(!strcmp(other.text, getenv("WITH_STRONG") ? "O" : ""));
    return record(event, 'T');
})
PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, { return record(event, 'V'); })
#endif
