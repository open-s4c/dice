#define DICE_MODULE_SLOT SLOT_SOURCE
#include "state.h"

PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_APP_FAST, {
    static unsigned serial;
    struct state *state = event;
    state->serial       = ++serial;
    state->order        = 1;
    if (state->stop)
        return PS_STOP_CHAIN;
    PS_PUBLISH(CHAIN_DONE, EVENT_DONE, state, md);
    if (state->feedback && !state->entered) {
        state->entered = true;
        PS_PUBLISH(CHAIN_FLOW, EVENT_TEST, state, md);
        state->entered = false;
    }
})

#ifndef DUPLICATE
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_APP_NORMAL, {
    struct state *state = event;
    state->order        = state->order * 10 + 2;
})
PS_SUBSCRIBE_SLOT(CHAIN_DONE, EVENT_DONE, SLOT_APP_NORMAL, {
    struct state *state = event;
    state->nested++;
})
#endif
