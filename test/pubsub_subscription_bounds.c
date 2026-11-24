/* SPDX-License-Identifier: 0BSD */
#include <dice/ensure.h>
#include <dice/events/dice.h>
#include <dice/pubsub.h>

static enum ps_err
count_event(chain_id chain, type_id type, void *event, struct metadata *md)
{
    (void)md;
    ensure(chain == MAX_CHAINS - 1 && type == MAX_TYPES - 1);
    ++*(int *)event;
    return PS_OK;
}

int
main(void)
{
    const int slot = 10000;
    ensure(ps_subscribe(CHAIN_DICE_CONTROL, MAX_TYPES, count_event, 0) ==
           PS_INVALID);
    ensure(ps_subscribe(MAX_CHAINS, EVENT_DICE_NOP, count_event, slot) ==
           PS_INVALID);
    ensure(ps_subscribe(MAX_CHAINS, ANY_EVENT, count_event, slot) ==
           PS_INVALID);

    ensure(ps_subscribe(MAX_CHAINS - 1, MAX_TYPES - 1, count_event, slot) ==
           PS_OK);
    int calls = 0;
    ensure(ps_publish(MAX_CHAINS - 1, MAX_TYPES - 1, &calls, NULL) == PS_OK);
    ensure(calls == 1);
    return 0;
}
