#include <assert.h>

#include "events.h"
#include <dice/chains/capture.h>
#include <dice/events/pthread.h>
#include <dice/pubsub.h>

STATIC_ASSERT(EVENT_PTHREAD_CREATE == EVENT_THREAD_CREATE, "event alias");
STATIC_ASSERT(EVENT_THREAD_CREATE == 3 && CAPTURE_EVENT == 4, "stable IDs");
#ifdef DICE_ROLLED
STATIC_ASSERT(EVENT_COMPAT == 128 && CHAIN_COMPAT == 7, "generated IDs");
#else
STATIC_ASSERT(CHAIN_CONTROL == CHAIN_DICE_CONTROL, "legacy control alias");
STATIC_ASSERT(EVENT_COMPAT == 200 && CHAIN_COMPAT == 11, "legacy IDs");
STATIC_ASSERT(MAX_TYPES == 256 && MAX_CHAINS == 12, "CMake definitions");
STATIC_ASSERT(LAST_DISPATCH_SLOT == 3, "CMake dispatch slots");
#endif

int
main(void)
{
    struct compat_event event = {0};
    assert(ps_publish(CHAIN_COMPAT, EVENT_COMPAT, &event, NULL) ==
           PS_STOP_CHAIN);
    assert(event.count == 1);
    return 0;
}
