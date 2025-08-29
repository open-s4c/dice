/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <dice/now.h>
#define NUM_THREADS 20


__attribute__((noinline)) void
fib(uint64_t *n, uint64_t *res)
{
    uint64_t buf_1 = 0;
    uint64_t buf_2 = 1;
    for (uint64_t i = 0; i < *n; i++) {
        uint64_t tmp = buf_2;
        buf_2        = buf_1 + buf_2;
        buf_1        = tmp;
    }
    *res = buf_1;
    return;
}

void *
fib_thread_func(void *ptr)
{
    uint64_t n = (uint64_t)ptr;
    uint64_t res;
    fib(&n, &res);
    return NULL;
}


int
main()
{
    pthread_t threads[NUM_THREADS];
    struct timespec sleep_time = to_timespec(500 * NOW_MILLISECOND);

    for (uint64_t i = 0; i < NUM_THREADS; i++) {
        pthread_create(threads + i, NULL, fib_thread_func, (void *)i);
        pthread_detach(threads[i]);
    }

    nanosleep(&sleep_time, NULL);
    return 0;
}
