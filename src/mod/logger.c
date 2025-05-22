/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <pthread.h>

// #define DICE_XTOR_PRIO 300
#include <dice/intercept.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>

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


DICE_MODULE_INIT()
