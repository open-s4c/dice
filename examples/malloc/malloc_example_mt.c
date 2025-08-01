#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *
run(void *ptr)
{
    printf("here\n");
        free(ptr);
    printf("done done\n");
    return 0;
}

int
main(void)
{
    pthread_t th;
    void *ptr = malloc(123);
    pthread_create(&th, 0, run, ptr);
    pthread_join(th, 0);
    printf("done\n");
    return 0;
}
