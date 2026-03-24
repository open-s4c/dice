#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/module.h>
#include <dice/self.h>

#define EVENT_SELF_GUARD_TEST 100

static vatomic32_t capture_event_seen;
static vatomic32_t capture_before_seen;
static vatomic32_t capture_after_seen;

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_SELF_GUARD_TEST, {
    ensure(self_guard_get(md) == SELF_GUARD_SERVING);
    vatomic32_inc(&capture_event_seen);
})

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_SELF_GUARD_TEST, {
    ensure(self_guard_get(md) == SELF_GUARD_SERVING);
    vatomic32_inc(&capture_before_seen);
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_SELF_GUARD_TEST, {
    ensure(self_guard_get(md) == SELF_GUARD_SERVING);
    vatomic32_inc(&capture_after_seen);
})

int
main(void)
{
    ensure(self_guard_get(NULL) == SELF_GUARD_NONE);
    ensure(self_guard_get(self_md()) == SELF_GUARD_NONE);

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_SELF_GUARD_TEST, NULL, NULL);
    ensure(vatomic_read(&capture_event_seen) == 1);
    ensure(self_guard_get(self_md()) == SELF_GUARD_NONE);

    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SELF_GUARD_TEST, NULL, NULL);
    ensure(vatomic_read(&capture_before_seen) == 1);
    ensure(self_guard_get(self_md()) == SELF_GUARD_BETWEEN);

    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SELF_GUARD_TEST, NULL, NULL);
    ensure(vatomic_read(&capture_after_seen) == 1);
    ensure(self_guard_get(self_md()) == SELF_GUARD_NONE);
    return 0;
}
