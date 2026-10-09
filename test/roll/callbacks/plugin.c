#define DICE_PLUGIN_MODULE
#define DICE_MODULE_SLOT SLOT_APP_NESTED_REOPENED
#include "dice.h"

void hit(char);

/* This slot opts in even when its module and the top level opt out. */
PS_SUBSCRIBE(CHAIN_FLOW, ANY_EVENT, { hit('D'); })
