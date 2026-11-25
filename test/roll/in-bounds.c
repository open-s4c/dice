#define DICE_MODULE_SLOT SLOT_PUBLISHER
#include <assert.h>
#include <string.h>

#include "dice.h"

static char hits[32];
static unsigned count;

static void
hit(char value)
{
    hits[count++] = value;
    hits[count]   = 0;
}

PS_SUBSCRIBE_SLOT(CHAIN_FLOW, ANY_EVENT, SLOT_LINKED, { hit('L'); })
PS_SUBSCRIBE_SLOT(CHAIN_FLOW, EVENT_DICE_NOP, SLOT_LINKED, { hit('E'); })

#if DICE_CALLBACKS
static unsigned stop_after;

static enum ps_err
wildcard(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    hit('W');
    return stop_after && count >= stop_after ? PS_STOP_CHAIN : PS_OK;
}

static enum ps_err
exact(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    hit('X');
    return PS_OK;
}
#endif

static enum ps_err
publish(unsigned path, type_id type)
{
#if !DICE_CHECK_ROUTES
    if (path == 0)
        return (ps_publish)(CHAIN_FLOW, type, NULL, NULL);
    if (path == 1)
        return dice_publish_internal_(CHAIN_FLOW, type, NULL, NULL);
    #if DICE_HEADER_DISPATCH
    if (path == 2)
        return dice_publish_inline_(CHAIN_FLOW, type, NULL, NULL);
    if (path == 3)
        return dice_publish_inline_nested_(CHAIN_FLOW, type, NULL, NULL);
    #endif
#endif
    (void)path;
    return ps_publish(CHAIN_FLOW, type, NULL, NULL);
}

int
main(void)
{
    /* An in-range gap succeeds even before any callbacks are registered. */
    assert(publish(4, 103) == PS_OK);
    assert(!strcmp(hits, "L"));
#if DICE_CALLBACKS
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, wildcard, SLOT_REGISTERED) ==
           PS_OK);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_DICE_NOP, exact, SLOT_REGISTERED) ==
           PS_OK);
    assert(ps_subscribe(CHAIN_FLOW, 103, exact, SLOT_REGISTERED) == PS_INVALID);
    #if DICE_EXTENSION_CALLBACKS
    assert(ps_subscribe(CHAIN_FLOW, 103, exact, 10) == PS_INVALID);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, wildcard, 10) == PS_OK);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, wildcard, 11) == PS_OK);
    #endif
#endif
    for (unsigned path = 0; path < 5; path++) {
        type_id valid[] = {1, 103, EVENT_HIGH - 1, EVENT_HIGH, EVENT_DICE_NOP};
        for (unsigned i = 0; i < sizeof(valid) / sizeof(valid[0]); i++) {
            count = 0;
            assert(publish(path, valid[i]) == PS_OK);
#if DICE_EXTENSION_CALLBACKS
            assert(!strcmp(hits, valid[i] == EVENT_DICE_NOP ? "EXWW" : "LWWW"));
#elif DICE_CALLBACKS
            assert(!strcmp(hits, valid[i] == EVENT_DICE_NOP ? "EX" : "LW"));
#else
            assert(!strcmp(hits, valid[i] == EVENT_DICE_NOP ? "E" : "L"));
#endif
        }
        count = 0;
        assert(publish(path, ANY_EVENT) == PS_INVALID);
        assert(publish(path, MAX_TYPES) == PS_INVALID);
        assert(publish(path, UINT16_MAX) == PS_INVALID);
        assert(count == 0);
#if DICE_CALLBACKS
        stop_after = 2;
        assert(publish(path, 103) == PS_STOP_CHAIN);
        assert(!strcmp(hits, "LW"));
    #if DICE_EXTENSION_CALLBACKS
        count      = 0;
        stop_after = 3;
        assert(publish(path, 103) == PS_STOP_CHAIN);
        assert(!strcmp(hits, "LWW"));
    #endif
        stop_after = 0;
#endif
    }
    return 0;
}
