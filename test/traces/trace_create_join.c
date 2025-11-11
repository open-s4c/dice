#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <trace_checker.h>

#include <dice/chains/capture.h>
#include <dice/events/memaccess.h>
#include <dice/events/self.h>
#include <dice/events/stacktrace.h>
#include <dice/events/thread.h>
#include <dice/log.h>

struct expected_event expected_1[] = {
#ifndef __APPLE__
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    // initialization until THREAD_START is published
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_THREAD_START),
#endif
    // there is a short time window until the main function is finally called,
    // some mutex might be accessed during this period
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_STACKTRACE_ENTER),
    // we should now be in the main function, register_expected_trace calls are
    // not instrumented. We should be able to match every event.
    EXPECTED_EVENT(CAPTURE_BEFORE, EVENT_THREAD_CREATE),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_THREAD_CREATE),
    // read the content of variable t
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_MA_READ),
    // and pass that to join
    EXPECTED_EVENT(CAPTURE_BEFORE, EVENT_THREAD_JOIN),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_THREAD_JOIN),
    // now exit main function
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_STACKTRACE_EXIT),
    // between the exit of the main function and the THREAD_EXIT of the main
    // thread, there might be calls done by atexit-callbacks.
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_THREAD_EXIT),
    // And between that and the destructor of Dice, there might be other calls
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    // Since we cannot capture the real end of the program, we must allow
    // any suffix in the main thread
    EXPECTED_ANY_SUFFIX,
};

struct expected_event expected_2[] = {
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_START),
    // The empty run() function does not get instrumented by TSAN, so we don't
    // need any STACKTRACE_ENTER/EXIT events
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_EXIT),
    // Between thread exit and self_fini, there might be further events in this
    // thread, so we have to only ensure the suffix
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    EXPECTED_END,
};

void *
run()
{
    return 0;
}

int
main()
{
    register_expected_trace(1, expected_1);
    register_expected_trace(2, expected_2);

    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);
    return 0;
}
