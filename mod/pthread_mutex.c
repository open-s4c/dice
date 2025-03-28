/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include "defs.h"
#include <bingo/intercept.h>
#include <bingo/mempool.h>
#include <bingo/real.h>

INTERPOSE(int, pthread_mutex_lock, pthread_mutex_t *mutex)
{
    intercept_before(EVENT_MUTEX_LOCK, 0, 0);
    int ret = REAL(pthread_mutex_lock, mutex);
    intercept_after(EVENT_MUTEX_LOCK, 0, 0);
    return ret;
}

INTERPOSE(int, pthread_mutex_unlock, pthread_mutex_t *mutex)
{
    intercept_before(EVENT_MUTEX_UNLOCK, 0, 0);
    int ret = REAL(pthread_mutex_unlock, mutex);
    intercept_after(EVENT_MUTEX_UNLOCK, 0, 0);
    return ret;
}

BINGO_MODULE_INIT()
