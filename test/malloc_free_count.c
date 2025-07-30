/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/events/malloc.h>
#include <dice/module.h>
#include <dice/pubsub.h>

int malloc_called;
int free_called;
int malloc_intercepted;
int free_intercepted;
int run_called;

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, { malloc_called++; })
PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_FREE, { free_called++; })
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MALLOC, { malloc_intercepted++; })
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_FREE, { free_intercepted++; })


__thread int threadint = 0;
void *
run(void* ptr)
{
    (void) ptr;
    threadint ++;
    run_called++;
    // free(ptr);
    return 0;
}

int
main()
{
    pthread_t t;
    void * ptr = malloc(42);
    threadint++;
    pthread_create(&t, 0, run, ptr);
    pthread_join(t, 0);
    threadint--;
    printf("malloc_called %d\n", malloc_called);
    printf("free_called %d\n", malloc_called);
    printf("malloc_intercepted %d\n", malloc_intercepted);
    printf("free_intercepted %d\n", free_intercepted);
    assert(run_called == 1);
    assert(malloc_called == free_called);
    assert(malloc_intercepted == free_intercepted);

    return 0;
}
