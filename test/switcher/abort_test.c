/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include <dice/ensure.h>
#include <dice/switcher.h>

int counter = 0;

void *
r1()
{
    int tid = 2;
    int yd  = switcher_yield(tid, true);
    if (yd == SWITCHER_ABORTED)
        return 0;
    switcher_abort();
    switcher_wake(ANY_THREAD, 0);
    ++counter;
    return 0;
}


void *
r2()
{
    int tid = 3;
    int yd  = switcher_yield(tid, true);
    if (yd == SWITCHER_ABORTED)
        return 0;
    switcher_abort();
    switcher_wake(ANY_THREAD, 0);
    ++counter;
    return 0;
}

int
main()
{
    pthread_t t[2];

    pthread_create(&t[0], 0, r1, 0);
    pthread_create(&t[1], 0, r2, 0);

    switcher_wake(ANY_THREAD, 0);


    pthread_join(t[0], 0);
    pthread_join(t[1], 0);

    printf("%d\n", counter);
    ensure(counter == 1);
}
