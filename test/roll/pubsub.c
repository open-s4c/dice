#define DICE_MODULE_SLOT SLOT_INITIALIZER
#include <assert.h>

#include "dice.h"

static int init_count, ready_count, nested_count;

PS_SUBSCRIBE(CHAIN_DICE_CONTROL, EVENT_DICE_INIT, {
    ++init_count;
    assert(ps_publish(CHAIN_LOW, EVENT_LOW, NULL, NULL) == PS_STOP_CHAIN);
})
PS_SUBSCRIBE(CHAIN_DICE_CONTROL, EVENT_DICE_READY, {
    ++ready_count;
    assert(ps_publish(CHAIN_LOW, EVENT_LOW, NULL, NULL) == PS_OK);
})
PS_SUBSCRIBE(CHAIN_LOW, EVENT_LOW, { ++nested_count; })

#if DICE_PLUGINS
static int trace, stop;

    #define CALLBACK(NAME, DIGIT)                                              \
        static enum ps_err NAME(chain_id chain, type_id type, void *event,     \
                                struct metadata *md)                           \
        {                                                                      \
            (void)chain;                                                       \
            (void)type;                                                        \
            (void)event;                                                       \
            (void)md;                                                          \
            trace = trace * 10 + DIGIT;                                        \
            return stop ? PS_STOP_CHAIN : PS_OK;                               \
        }

CALLBACK(early, 1)
CALLBACK(exact, 2)
CALLBACK(wildcard, 3)
CALLBACK(wildcard2, 4)
CALLBACK(late, 5)

static void
check_delivery(chain_id chain, type_id type, int expected, enum ps_err result)
{
    trace = 0;
    assert(ps_publish(chain, type, NULL, NULL) == result);
    assert(trace == expected);
}
#endif

int
main(void)
{
    assert(init_count == 1 && ready_count == 1 && nested_count == 1);
    assert(ps_publish(CHAIN_LOW, EVENT_LOW, NULL, NULL) == PS_OK);
    assert(init_count == 1 && ready_count == 1 && nested_count == 2);
#if DICE_PLUGINS
    /* Exact callbacks precede wildcards at equal slots, in either registration
     * order. Callbacks remain separated by chain and event. */
    assert(ps_subscribe(CHAIN_HIGH, ANY_EVENT, wildcard, 20) == PS_OK);
    assert(ps_subscribe(CHAIN_HIGH, EVENT_HIGH, exact, 20) == PS_OK);
    assert(ps_subscribe(CHAIN_HIGH, ANY_EVENT, wildcard2, 20) == PS_OK);
    assert(ps_subscribe(CHAIN_HIGH, EVENT_HIGH, late, 30) == PS_OK);
    assert(ps_subscribe(CHAIN_HIGH, EVENT_HIGH, early, 10) == PS_OK);
    assert(ps_subscribe(CHAIN_LOW, EVENT_LOW, exact, 20) == PS_OK);
    assert(ps_subscribe(CHAIN_LOW, ANY_EVENT, wildcard, 20) == PS_OK);
    check_delivery(CHAIN_HIGH, EVENT_HIGH, 12345, PS_OK);
    check_delivery(CHAIN_HIGH, EVENT_LOW, 34, PS_OK);
    check_delivery(CHAIN_LOW, EVENT_LOW, 23, PS_OK);
    check_delivery(CHAIN_LOW, EVENT_HIGH, 3, PS_OK);
    stop = 1;
    check_delivery(CHAIN_HIGH, EVENT_HIGH, 1, PS_STOP_CHAIN);

    assert(ps_subscribe(MAX_CHAINS, EVENT_HIGH, exact, 20) == PS_INVALID);
    assert(ps_subscribe(CHAIN_HIGH, MAX_TYPES, exact, 20) == PS_INVALID);
    assert(ps_subscribe(CHAIN_HIGH, EVENT_HIGH, NULL, 20) == PS_INVALID);
    assert(ps_subscribe(CHAIN_HIGH, EVENT_HIGH, exact, -1) == PS_INVALID);
#endif
    puts("pubsub passed");
}
