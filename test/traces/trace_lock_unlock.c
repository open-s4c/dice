#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <trace_checker.h>

#include <dice/chains/capture.h>
#include <dice/events/memaccess.h>
#include <dice/events/pthread.h>
#include <dice/events/self.h>
#include <dice/events/thread.h>
#include <dice/log.h>


struct expected_event expected[] = {
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_THREAD_START),
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_LOCK),

    // when checking coverage, these are also triggered
    EXPECTED_SOME(CAPTURE_EVENT, EVENT_MA_READ, 0, 1),
    EXPECTED_SOME(CAPTURE_EVENT, EVENT_MA_WRITE, 0, 1),
    // end

    EXPECTED_EVENT(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_UNLOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_UNLOCK),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    EXPECTED_ANY_SUFFIX,
    EXPECTED_END,
};

int
main()
{
    register_expected_trace(1, expected);

    pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&l);
    pthread_mutex_unlock(&l);
    return 0;
}
