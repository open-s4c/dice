/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/ensure.h>
#include <pthread.h>
#include <stdbool.h>

#include <dice/ensure.h>
#include <dice/switcher.h>

int counter = 0;
int pid     = 0;

void *
r1()
{
    int yd = switcher_yield(2, true);
    ensure(yd != SWITCHER_ABORTED);
    ++counter;
    switcher_wake(ANY_THREAD, 0);
    return 0;
}


void *
r2()
{
    int yd = switcher_yield(3, true);
    ensure(yd != SWITCHER_ABORTED);
    ++counter;
    switcher_wake(ANY_THREAD, 0);
    return 0;
}

void *
r3()
{
    int yd = switcher_yield(4, true);
    ensure(yd != SWITCHER_ABORTED);
    ++counter;
    switcher_wake(5, 0);
    return 0;
}

void *
r4()
{
    int yd = switcher_yield(5, false);
    ensure(yd != SWITCHER_ABORTED);
    ++counter;
    switcher_wake(ANY_THREAD, 0);
    return 0;
}


int
main()
{
    pthread_t t[4];

    pthread_create(&t[0], 0, r1, 0);
    pthread_create(&t[1], 0, r2, 0);
    pthread_create(&t[2], 0, r3, 0);
    pthread_create(&t[3], 0, r4, 0);


    switcher_wake(ANY_THREAD, 0);


    pthread_join(t[0], 0);
    pthread_join(t[1], 0);
    pthread_join(t[2], 0);
    pthread_join(t[3], 0);

    ensure(counter == 4);
}
