/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_PTHREAD_H
#define BINGO_PTHREAD_H

#include <bingo/thread_id.h>

enum pthread_events {
    EVENT_THREAD_CREATE  = 200,
    EVENT_THREAD_JOIN    = 201,
    EVENT_MUTEX_LOCK     = 300,
    EVENT_MUTEX_UNLOCK   = 301,
    EVENT_MUTEX_TRYLOCK  = 302,
    EVENT_COND_WAIT      = 401,
    EVENT_COND_TIMEDWAIT = 402,
    EVENT_COND_SIGNAL    = 403,
    EVENT_COND_BROADCAST = 404,
};

struct pthread_mutex_event {
    void *mutex;
    int ret;
};

struct pthread_cond_event {
    void *cond;
    void *mutex;
    int ret;
};

#endif /* BINGO_PTHREAD_H */
