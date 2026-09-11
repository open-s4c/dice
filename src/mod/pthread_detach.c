/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(int, pthread_detach, pthread_t thread)
{
    struct pthread_detach_event ev = {
        .pc   = INTERPOSE_PC,
        .thread = thread,
        .ret  = 0,
        .func = REAL_FUNC(pthread_detach),
    };

    metadata_t md = {0};

    PS_PUBLISH(
        INTERCEPT_BEFORE,
        EVENT_PTHREAD_DETACH,
        &ev,
        &md
    );

    ev.ret = ev.func(ev.thread);

    PS_PUBLISH(
        INTERCEPT_AFTER,
        EVENT_PTHREAD_DETACH,
        &ev,
        &md
    );

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_DETACH)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
