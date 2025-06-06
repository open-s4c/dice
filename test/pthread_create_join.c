#include <assert.h>
#include <pthread.h>

#include <dice/intercept/pthread.h>
#include <dice/thread_id.h>

int init_called;
int fini_called;
int run_called;

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_INIT, { init_called++; })
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_FINI, { fini_called++; })

void *
run()
{
    run_called++;
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);

    assert(run_called == 1);
    assert(init_called == 1);
    assert(fini_called == 1);

    return 0;
}
