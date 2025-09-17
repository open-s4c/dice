/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/ensure.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <dice/ensure.h>
#include <dice/switcher.h>

int counter = 0;
int pid     = 0;

time_t start, finish;


void *
r1()
{
    sleep(2); // Forces yield of r2 to execute while the next is still r1
              // thread, running the nanosleep.
    printf("Entered thread 1\n");
    int yd = switcher_yield(2, false);
    if (yd == SWITCHER_ABORTED)
        return 0;
    ++counter;
    ensure(counter == 1);
    switcher_wake(3, (nanosec_t)(2.0 * NOW_SECOND));
    return 0;
}


void *
r2()
{
    printf("Entered thread 2\n");
    int yd = switcher_yield(3, false);
    if (yd == SWITCHER_ABORTED)
        return 0;
    ++counter;
    ensure(counter == 2);
    return 0;
}


int
main()
{
    double elapsed;
    start = time(NULL);

    switcher_wake(2, (nanosec_t)(1.5 * NOW_SECOND));

    pthread_t t[2];
    pthread_create(&t[0], 0, r1, 0);
    pthread_create(&t[1], 0, r2, 0);

    pthread_join(t[0], 0);
    pthread_join(t[1], 0);

    finish  = time(NULL);
    elapsed = (double)(finish - start);
    printf("elapsed time = %.2lf", elapsed);

    ensure(counter == 2);
    ensure((elapsed - 4.00) <= 1e-6); // should be >= sleep + slack time
}
