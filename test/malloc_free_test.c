/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include <vsync/atomic.h>

vatomic32_t count = VATOMIC_INIT(4);

void *
run(void *arg)
{
    if (!arg)
        return 0;

    pthread_t t;
    pthread_create(&t, 0, run, vatomic_dec_get(&count) > 0 ? malloc(44) : 0);
    free(arg);
    pthread_join(t, 0);
    return 0;
}

int
main(void)
{
    pthread_t t;
    pthread_create(&t, 0, run, malloc(42));
    pthread_join(t, 0);
    return 0;
}
