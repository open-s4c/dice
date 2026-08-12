/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(int, pthread_once, pthread_once_t *once_control, void (*init_routine)(void))
{
    struct pthread_once_event ev = {
        .pc           = INTERPOSE_PC,
        .once_control = once_control,
        .init_routine = init_routine,
        .ret          = 0,
        .func         = REAL_FUNC(pthread_once),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PTHREAD_ONCE, &ev, &md);

    ev.ret = ev.func(ev.once_control, ev.init_routine);

    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PTHREAD_ONCE, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_ONCE)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
