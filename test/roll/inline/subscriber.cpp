#define DICE_MODULE_SLOT SLOT_FAST
extern "C" {
#include "dice.h"

PS_SUBSCRIBE(CHAIN_FLOW, EVENT_TEST, { ++*(unsigned *)event; })
#ifndef PARTIAL
PS_SUBSCRIBE(CHAIN_FLOW, EVENT_OTHER, { *(unsigned *)event += 2; })
#endif
}
