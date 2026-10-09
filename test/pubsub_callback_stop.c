#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/dice.h>
#include <dice/pubsub.h>

struct event {
    int calls;
    bool stop;
};

static enum ps_err
callback(chain_id chain, type_id type, void *payload, struct metadata *md)
{
    (void)md;
    ensure(chain == CAPTURE_EVENT && type == EVENT_DICE_NOP);
    struct event *event = payload;
    event->calls++;
    return event->stop && event->calls == 2 ? PS_STOP_CHAIN : PS_OK;
}

int
main(void)
{
    struct event event = {0};
    ensure(ps_publish(CAPTURE_EVENT, EVENT_DICE_NOP, &event, NULL) == PS_OK);
    for (int slot = 10000; slot < 10003; slot++)
        ensure(ps_subscribe(CAPTURE_EVENT, EVENT_DICE_NOP, callback, slot) ==
               PS_OK);

    event.stop = true;
    ensure(ps_publish(CAPTURE_EVENT, EVENT_DICE_NOP, &event, NULL) == PS_OK);
    ensure(event.calls == 2);

    event = (struct event){0};
    ensure(ps_publish(CAPTURE_EVENT, EVENT_DICE_NOP, &event, NULL) == PS_OK);
    ensure(event.calls == 3);
    return 0;
}
