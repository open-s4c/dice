#include <trace_checker.h>

#include <dice/chains/capture.h>
#include <dice/events/memaccess.h>
#include <dice/events/self.h>

struct expected_event trace[] = {
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_MA_READ),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    EXPECTED_ANY_SUFFIX,
    EXPECTED_END,
};

volatile uint64_t x = 0;
int
main()
{
    register_expected_trace(1, trace);
    return x;
}
