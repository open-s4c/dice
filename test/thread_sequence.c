#include <pthread.h>
#include <vsync/atomic.h>

#define NTHREADS 16
vatomic32_t ready;

void *
run(void *_)
{
    (void)_;
    vatomic32_inc(&ready);
    return 0;
}

int
main(void)
{
    pthread_t t;
    for (int i = 0; i < NTHREADS; i++) {
        pthread_create(&t, 0, run, 0);
        pthread_join(t, 0);
    }
    return 0;
}
