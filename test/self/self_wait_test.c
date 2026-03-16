#include <pthread.h>

#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/events/self.h>
#include <dice/module.h>

#define NTHREADS 20
pthread_t t[NTHREADS];


void *
run(void *arg)
{
    (void)arg;
    PS_PUBLISH(INTERCEPT_EVENT, 100, 0, 0);
    while (true)
        sched_yield();
    return NULL;
}


int
main(void)
{
    PS_PUBLISH(INTERCEPT_EVENT, 100, 0, 0);
    for (int i = 0; i < NTHREADS; ++i) {
        pthread_create(&t[i], NULL, run, NULL);
    }
    return 0;
    /* Do not wait for threads to exit. */
}
