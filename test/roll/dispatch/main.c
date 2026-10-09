#define DICE_MODULE_SLOT SLOT_SOURCE
#include <assert.h>
#include <string.h>

#include "trace.h"

STATIC_ASSERT(DICE_LAST_KNOWN_SLOT == 10, "global slot boundary");

static enum ps_err
dynamic(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)md;
    return record(event, 'b');
}

#if DICE_CALLBACKS
static enum ps_err
duplicate(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    assert(0 && "rejected handler must never run");
    return PS_ERROR;
}
#endif

static void
check(type_id type, const char *expected, char stop)
{
    struct trace trace = {.stop = stop};
    enum ps_err result = ps_publish(CHAIN_FLOW, type, &trace, NULL);
    if (strcmp(trace.text, expected))
        fprintf(stderr, "expected %s, got %s\n", expected, trace.text);
    assert(!strcmp(trace.text, expected));
    assert(result == (stop ? PS_STOP_CHAIN : PS_OK));
}

int
main(int argc, char **argv)
{
    if (argc > 1) {
#if DICE_CALLBACKS
        if (!strcmp(argv[1], "duplicates")) {
            bool strong = getenv("WITH_STRONG") != NULL;
            if (strong) {
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, duplicate,
                                    SLOT_DIRECT) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, duplicate,
                                    SLOT_LAST) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, duplicate,
                                    SLOT_LAST) == PS_INVALID);
                /* A linked wildcard also covers undeclared exact cases. */
                assert(ps_subscribe(CHAIN_FLOW, EVENT_OTHER, duplicate,
                                    SLOT_LAST) == PS_INVALID);
                check(EVENT_TEST, "DL", 0);
                check(EVENT_OTHER, "W", 0);
            } else {
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic,
                                    SLOT_EARLY) == PS_OK);
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic,
                                    SLOT_EARLY) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, duplicate,
                                    SLOT_EARLY) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, dynamic,
                                    SLOT_EARLY) == PS_OK);
                assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, dynamic,
                                    SLOT_EARLY) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, duplicate,
                                    SLOT_EARLY) == PS_INVALID);
                /* The same slot on another chain is independently occupied. */
                assert(ps_subscribe(CHAIN_OTHER, EVENT_OTHER, dynamic,
                                    SLOT_EARLY) == PS_OK);
                /* A different event at the same slot is also independent. */
                assert(ps_subscribe(CHAIN_FLOW, EVENT_OTHER, dynamic,
                                    SLOT_EARLY) == PS_OK);
                assert(ps_subscribe(CHAIN_FLOW, EVENT_OTHER, duplicate,
                                    SLOT_EARLY) == PS_INVALID);
                assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic,
                                    SLOT_DIRECT) == PS_OK);
                check(EVENT_TEST, "bb", 0);
                check(EVENT_OTHER, "bb", 0);
            }
            return 0;
        }
        if (!strcmp(argv[1], "extensions")) {
            int slot = DICE_LAST_KNOWN_SLOT + 1;
            assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, slot) ==
                   PS_OK);
            assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, slot) ==
                   PS_OK);
            assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, dynamic, slot) == PS_OK);
            check(EVENT_TEST, getenv("WITH_STRONG") ? "DLbbb" : "bbb", 0);
            return 0;
        }
        if (!strcmp(argv[1], "wildcard")) {
            /* A declared but unregistered exact callback still excludes ANY.
             */
            bool strong = getenv("WITH_STRONG") != NULL;
            assert(ps_subscribe(CHAIN_FLOW, ANY_EVENT, dynamic, SLOT_EARLY) ==
                   PS_OK);
            check(EVENT_TEST, strong ? "DL" : "", 0);
            check(EVENT_OTHER, strong ? "bW" : "b", 0);
            return 0;
        }
#endif
        int slot   = !strcmp(argv[1], "zero") ? 0 :
                     !strcmp(argv[1], "gap")  ? 9 :
                                                DICE_LAST_KNOWN_SLOT;
        int result = ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, slot);
        assert(result == PS_INVALID);
        return 0;
    }
    char expected[64];
#if DICE_CALLBACKS
    bool plugin      = getenv("WITH_PLUGIN") != NULL;
    bool strong      = getenv("WITH_STRONG") != NULL;
    const char *tail = plugin && DICE_CALLBACKS ? "TUV" : "";
    snprintf(expected, sizeof(expected), "%s%s",
             plugin ? (strong ? "ADBL" : "AxBy") : (strong ? "DL" : ""), tail);
    check(EVENT_TEST, expected, 0);

    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, SLOT_MIDDLE) == PS_OK);
    snprintf(expected, sizeof(expected), "%s%s",
             plugin ? (strong ? "ADbBL" : "AxbBy") : (strong ? "DbL" : "b"),
             tail);
    check(EVENT_TEST, expected, 0);
#else
    strcpy(expected, "ADBL");
    check(EVENT_TEST, expected, 0);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, SLOT_MIDDLE) ==
           PS_INVALID);
#endif
    /* Stop at every actual handler, including both sides of the phase boundary.
     */
    for (size_t i = 0; expected[i]; i++) {
        char prefix[64];
        memcpy(prefix, expected, i + 1);
        prefix[i + 1] = 0;
        check(EVENT_TEST, prefix, expected[i]);
    }
#if DICE_CALLBACKS
    check(EVENT_OTHER, plugin ? (strong ? "aBW" : "aBz") : (strong ? "W" : ""),
          0);
#else
    check(EVENT_OTHER, "aBW", 0);
#endif
    /* The boundary is global, including on an otherwise unsubscribed chain. */
#if !DICE_CHECK_ROUTES
    assert(ps_subscribe(CHAIN_OTHER, EVENT_OTHER, dynamic, 9) == PS_INVALID);
    assert(ps_subscribe(CHAIN_FLOW, EVENT_TEST, dynamic, -1) == PS_INVALID);
#endif
    puts("two phases passed");
}
