#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <trace_checker.h>

#include <dice/chains/capture.h>
#include <dice/events/self.h>
#include <dice/log.h>

struct expected_event expected[] = {
#ifndef __APPLE__
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
#endif
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    EXPECTED_ANY_SUFFIX,
    EXPECTED_END,
};

int x;

int
main()
{
    register_expected_trace(1, expected);

    x = 120;               // WRITE
    log_printf("%d\n", x); // READ
    return 0;
}
