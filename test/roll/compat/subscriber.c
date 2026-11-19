#include "events.h"
#include <dice/module.h>

PS_SUBSCRIBE(CHAIN_COMPAT, EVENT_COMPAT, {
    struct compat_event *ev = event;
    ev->count++;
    return PS_STOP_CHAIN;
})
DICE_MODULE_INIT()
