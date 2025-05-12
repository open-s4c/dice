/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include <vsync/atomic.h>

int data;
vatomic32_t ready;

void *
run0(void *_)
{
    (void)_;
    vatomic32_write(&ready, 1);
    data = 1;
    return 0;
}

void *
run1(void *_)
{
    (void)_;
    printf("here\n");
    if (vatomic32_read(&ready) == 1)
        assert(data == 1);
    printf("there\n");
    return 0;
}

int
main(void)
{
    pthread_t t0, t1;
    pthread_create(&t0, 0, run0, 0);
    pthread_create(&t1, 0, run1, 0);
    pthread_join(t0, 0);
    pthread_join(t1, 0);
    return 0;
}
