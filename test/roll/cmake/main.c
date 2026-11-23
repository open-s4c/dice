#include <pthread.h>
#include <stdlib.h>

int configured_slot(void);
int configured_event(void);
int observed_allocations(void);
int observed_starts(void);
int observed_exits(void);

static void *
worker(void *arg)
{
    return arg;
}

int
main(int argc, char **argv)
{
    if (argc != 3 || configured_slot() != atoi(argv[1]) ||
        configured_event() != atoi(argv[2]))
        return 1;
    int allocations = observed_allocations();
    void *memory    = malloc(1234);
    if (!memory || observed_allocations() != allocations + 1)
        return 2;
    free(memory);
    int starts = observed_starts(), exits = observed_exits();
    pthread_t thread;
    if (pthread_create(&thread, NULL, worker, NULL) ||
        pthread_join(thread, NULL))
        return 3;
    return observed_starts() != starts + 1 || observed_exits() != exits + 1;
}
