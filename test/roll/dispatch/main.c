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
#if DICE_PLUGINS
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
#if DICE_PLUGINS
    bool plugin      = getenv("WITH_PLUGIN") != NULL;
    bool strong      = getenv("WITH_STRONG") != NULL;
    const char *tail = plugin && DICE_PLUGINS ? "TUV" : "";
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
#if DICE_PLUGINS
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
