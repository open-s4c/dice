#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include <dice/chains/intercept.h>
#include <dice/events/memaccess.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/now.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <vsync/atomic.h>

int x = 0;
int y = 0;
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    struct ma_awrite_event *ev = EVENT_PAYLOAD(ev);
    x += ev->val.u64;
    y++;
    return PS_STOP_CHAIN;
})

struct thread_state {
    int x;
};

struct thread_state key_;

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AREAD, {
    struct ma_aread_event *ev  = EVENT_PAYLOAD(ev);
    struct thread_state *state = SELF_TLS(md, &key_);
    state->x += ev->val.u64;
    y++;
    return PS_STOP_CHAIN;
})

void
intercept(const chain_id chain, const type_id type, void *event,
          struct metadata *md)
{
    PS_PUBLISH(chain, type, event, md);
}

DICE_MODULE_INIT()
DICE_MODULE_FINI({ log_printf("count: %d\tsum: %d\n", y, x); })
