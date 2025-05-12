/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>

// #define BINGO_XTOR_PRIO 300
#include <bingo/intercept.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>

PS_SUBSCRIBE(
    CAPTURE_EVENT, ANY_TYPE,
    {
        // log_printf("%" PRIx64 "\t%u[%" PRIx64 "]\n", token._v, event,
        //         (uint64_t)pthread_self());
    })
PS_SUBSCRIBE(
    CAPTURE_AFTER, ANY_TYPE,
    {
        // log_printf("%" PRIx64 "\t%u[%" PRIx64 "]\n", token._v, event,
        //         (uint64_t)pthread_self());
    })
PS_SUBSCRIBE(
    CAPTURE_BEFORE, ANY_TYPE,
    {
        // log_printf("%" PRIx64 "\t%u[%" PRIx64 "]\n", token._v, event,
        //         (uint64_t)pthread_self());
    })


BINGO_MODULE_INIT()
