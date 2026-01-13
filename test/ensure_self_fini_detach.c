#include <pthread.h>

#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/self.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <vsync/atomic.h>

#define NTHREADS 10

vatomic32_t fini_called;
vatomic32_t run_called;

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_SELF_FINI, {
    if (self_id(md) == MAIN_THREAD) {
        ensure(vatomic_read(&run_called) == NTHREADS);
        ensure(vatomic_read(&fini_called) == NTHREADS);
    }

    vatomic_inc(&fini_called);
})

void *
run()
{
    vatomic_inc(&run_called);
    return 0;
}

int
main()
{
    pthread_t t[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        pthread_create(&t[i], 0, run, 0);
        pthread_detach(t[i]);
    }

    // cannot ensure fini_called nor run_called until main thread is about to
    // finish in SELF_FINI callback above

    return 0;
}
