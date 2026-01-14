#include <pthread.h>
#include <unistd.h>

#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/self.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <vsync/atomic.h>

void *
run()
{
    usleep(100000);
    sched_yield();
    exit(222);
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);

    return 0;
}
