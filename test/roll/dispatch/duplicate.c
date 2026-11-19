#define DICE_PLUGIN_MODULE
#define DICE_MODULE_SLOT SLOT_DIRECT
#include "trace.h"

PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, { return record(event, '!'); })
