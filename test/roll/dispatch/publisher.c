#define DICE_MODULE_SLOT SLOT_SOURCE
#include <assert.h>

#include "trace.h"

/* Linked module publication must reside inside the runtime: its core and
 * handler entry points deliberately have hidden visibility. */
void
publish_test(type_id type, struct trace *trace, bool dynamic_chain)
{
    if (dynamic_chain) {
        volatile chain_id chain = CHAIN_FLOW;
        PS_PUBLISH(chain++, type, trace, NULL);
        assert(chain == CHAIN_FLOW + 1);
    } else {
        PS_PUBLISH(CHAIN_FLOW, type, trace, NULL);
    }
}
