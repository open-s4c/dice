#include <assert.h>
#include <string.h>

#define DICE_MODULE_SLOT SLOT_PUBLISHER
#include "dice.h"

extern char hits[];
extern unsigned nhits;
void hit(char);
static bool stop;

static enum ps_err
callback(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)event;
    (void)md;
    hit(type == EVENT_TEST ? 'B' : 'b');
    return stop ? PS_STOP_CHAIN : PS_OK;
}

static enum ps_err
top(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    hit('E');
    return PS_OK;
}

#if DICE_EXTENSION_CALLBACKS
static enum ps_err
extension(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    hit('G');
    return PS_OK;
}
#endif

int
main(int argc, char **argv)
{
    (void)argv;
    if (argc > 1)
        return ps_subscribe(CHAIN_FLOW, EVENT_TEST, top,
                            DICE_LAST_KNOWN_SLOT + 1);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, callback, SLOT_APP_OPEN) ==
           PS_OK);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, callback, SLOT_APP_OPEN) ==
           PS_OK);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, callback, SLOT_APP_OPEN) ==
           PS_INVALID);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, top, SLOT_APP_CLOSED) ==
           PS_INVALID);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, top, SLOT_APP_NESTED_CLOSED) ==
           PS_INVALID);
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, top, SLOT_APP_LINKED) ==
           PS_INVALID);
#if DICE_EXTENSION_CALLBACKS
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, top, SLOT_TOP) == PS_OK);
    for (int i = 0; i < 2; i++)
        assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, extension,
                            DICE_LAST_KNOWN_SLOT + 1) == PS_OK);
    const char *tail = "GG";
#else
    assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, top, SLOT_TOP) == PS_INVALID);
    const char *tail = "";
#endif
    assert(ps_publish(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_OK);
    assert(!strncmp(hits, "ABCDEF", 6) && !strcmp(hits + 6, tail));
    nhits = 0;
    assert(ps_publish(CHAIN_FLOW, EVENT_OTHER, NULL, NULL) == PS_OK);
    assert(!strncmp(hits, "AbCDEF", 6) && !strcmp(hits + 6, tail));
    nhits = 0;
    stop  = true;
    assert(ps_publish(CHAIN_FLOW, EVENT_TEST, NULL, NULL) == PS_STOP_CHAIN);
    assert(!strcmp(hits, "AB"));
    return 0;
}
