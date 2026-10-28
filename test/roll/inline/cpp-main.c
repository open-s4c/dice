#define DICE_MODULE_SLOT SLOT_SOURCE
#include <assert.h>

#include "dice.h"

int
main(void)
{
    unsigned count = 0;
    assert(ps_publish(CHAIN_FLOW, EVENT_TEST, &count, NULL) == PS_OK);
    assert(count == 1);
    assert(ps_publish(CHAIN_FLOW, EVENT_OTHER, &count, NULL) == PS_OK);
    assert(count == 3);
    return 0;
}
