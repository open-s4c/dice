#include <pthread.h>
#include <stdlib.h>

static void *
worker(void *arg)
{
    return arg;
}

int
main(void)
{
    void *memory = malloc(1234);
    if (!memory)
        return 1;
    free(memory);
    pthread_t thread;
    if (pthread_create(&thread, NULL, worker, NULL))
        return 1;
    return pthread_join(thread, NULL);
}
