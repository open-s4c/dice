#define DICE_MODULE_SLOT SLOT_SOURCE
#include "state.h"

void
publish_constant(struct state *state)
{
    PS_PUBLISH(CHAIN_FLOW, EVENT_TEST, state, NULL);
}

void
publish_dynamic(struct state *state)
{
    volatile chain_id chain = CHAIN_FLOW;
    PS_PUBLISH(chain, EVENT_TEST, state, NULL);
}

enum ps_err
invoke_handler(struct state *state)
{
    /* Taking the address requires an out-of-line definition, even with LTO.
     * Its function-local static must be shared with all inlined calls. */
    ps_callback_f volatile handler =
        DICE_SLOT_HANDLER(CHAIN_FLOW, ANY_EVENT, SLOT_APP_FAST);
    return handler(CHAIN_FLOW, EVENT_TEST, state, NULL);
}
