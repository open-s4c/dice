#define DICE_MODULE_SLOT SLOT_FIRST
#include <assert.h>

#include "dice.h"

/* Public calls from a fast-path module must still reject invalid IDs. */
void
check_public_publish(void)
{
#if !DICE_CHECK_ROUTES
    ps_callback_f publish = ps_publish;
    assert(ps_publish(CHAIN_TEST, ANY_EVENT, NULL, NULL) == PS_INVALID);
    assert(ps_publish(MAX_CHAINS, EVENT_TEST, NULL, NULL) == PS_INVALID);
    assert(ps_publish(CHAIN_TEST, MAX_TYPES, NULL, NULL) == PS_INVALID);
    assert(publish(CHAIN_TEST, ANY_EVENT, NULL, NULL) == PS_INVALID);
    assert(publish(MAX_CHAINS, EVENT_TEST, NULL, NULL) == PS_INVALID);
    assert(publish(CHAIN_TEST, MAX_TYPES, NULL, NULL) == PS_INVALID);
#endif
}

PS_SUBSCRIBE(CHAIN_TEST, EVENT_TEST, {
    *(int *)event += 1;
    PS_PUBLISH(CHAIN_MIDDLE, type, event, md);
})

/* Publications must be attributed to LATER, even in FIRST's translation unit.
 */
PS_SUBSCRIBE_SLOT(CHAIN_MIDDLE, EVENT_TEST, SLOT_LATER, {
    *(int *)event += 1;
    PS_PUBLISH(CHAIN_OUTPUT, type, event, md);
})
