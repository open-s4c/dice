#define DICE_MODULE_SLOT SLOT_APP_CLOSED
#include "dice.h"

char hits[32];
unsigned nhits;

void
hit(char value)
{
    hits[nhits++] = value;
    hits[nhits]   = 0;
}

PS_SUBSCRIBE(CHAIN_FLOW, ANY_EVENT, { hit('A'); })

#ifndef OMIT_CLOSED
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_APP_NESTED_CLOSED, { hit('C'); })
#endif

#if !DICE_EXTENSION_CALLBACKS
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_TOP, { hit('E'); })
#endif

/* Enabling callbacks still permits a linked handler. */
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_APP_LINKED, { hit('F'); })
