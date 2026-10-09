#include <assert.h>

#include "events.h"
#include <dice/chains/capture.h>
#include <dice/events/pthread.h>
#include <dice/module.h>
#include <dice/pubsub.h>

PS_ADVERTISE_CHAIN(CHAIN_CONTROL)

STATIC_ASSERT(EVENT_PTHREAD_CREATE == EVENT_THREAD_CREATE, "event alias");
STATIC_ASSERT(EVENT_THREAD_CREATE == 3 && CAPTURE_EVENT == 4, "stable IDs");
#ifdef DICE_ROLLED
STATIC_ASSERT(EVENT_COMPAT == 128 && CHAIN_COMPAT == 7, "generated IDs");
#else
STATIC_ASSERT(EVENT_COMPAT == 200 && CHAIN_COMPAT == 11, "legacy IDs");
STATIC_ASSERT(MAX_TYPES == 256 && MAX_CHAINS == 12, "CMake definitions");
STATIC_ASSERT(LAST_DISPATCH_SLOT == 3, "CMake dispatch slots");
#endif

#ifndef DICE_ROLLED
static enum ps_err
callback(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)md;
    (*(int *)event)++;
    return PS_OK;
}
#endif

int
main(void)
{
    assert(ps_chain_lookup("CHAIN_CONTROL") == CHAIN_CONTROL);
#ifndef DICE_ROLLED
    /* Ordinary dispatch slots ignore callback registration, including NULL.
     * Callback-only chains retain their existing signed slot ordering. */
    assert(ps_subscribe(CHAIN_CONTROL, EVENT_DICE_NOP, NULL, 2) == PS_OK);
    assert(ps_subscribe(CAPTURE_AFTER, EVENT_DICE_NOP, callback, -1) == PS_OK);
    int calls = 0;
    assert(ps_publish(CAPTURE_AFTER, EVENT_DICE_NOP, &calls, NULL) == PS_OK);
    assert(calls == 1);
#endif
    struct compat_event event = {0};
    assert(ps_publish(CHAIN_COMPAT, EVENT_COMPAT, &event, NULL) ==
           PS_STOP_CHAIN);
    assert(event.count == 1);
    return 0;
}
