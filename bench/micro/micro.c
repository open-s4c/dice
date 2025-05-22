/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <dice/intercept/memaccess.h>
#include <dice/now.h>
#include <vsync/atomic.h>

vatomic32_t stop;
vatomic32_t start;
vatomic64_t count;

void *
run_time(void *_)
{
    (void)_;
    while (!vatomic_read_rlx(&start)) {}
    while (!vatomic_read_rlx(&stop)) {
        PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_AWRITE, 0, 0);
        vatomic_inc_rlx(&count);
    }

    return 0;
}

void *
run_count(void *_)
{
    (void)_;
    while (!vatomic_read_rlx(&start)) {}
    for (size_t i = 0; i < 1000000000; i++) {
        PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_AWRITE, 0, 0);
        vatomic_inc_rlx(&count);
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    if (argc > 3) {
        printf("Usage: %s [threads] [seconds]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nthreads = 1;
    int time     = 1;
    if (argc > 1) {
        nthreads = atoi(argv[1]);
    }

    pthread_t *t = malloc(sizeof(pthread_t) * nthreads);

    if (argc > 2) {
        time = atoi(argv[2]);
        for (int i = 0; i < nthreads; i++)
            pthread_create(&t[i], 0, run_time, 0);
    } else {
        for (int i = 0; i < nthreads; i++)
            pthread_create(&t[i], 0, run_count, 0);
    }

    nanosec_t ts = now();
    vatomic_write(&start, 1);
    if (argc > 2) {
        sleep(time);
        vatomic_write(&stop, 1);
    }
    for (int i = 0; i < nthreads; i++)
        pthread_join(t[i], 0);
    nanosec_t te = now();

    printf("threads=%d count=%" PRIu64 " elapsed=%.2fs\n", nthreads,
           vatomic_read(&count), in_sec(te - ts));
    free(t);
    return EXIT_SUCCESS;
}
