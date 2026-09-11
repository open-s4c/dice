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
    struct before_pthread_once_event ev = {
        .pc           = INTERPOSE_PC,
        .once_control = once_control,
        .init_routine = init_routine,
        .ret          = 0,
        .func         = REAL_FUNC(pthread_once),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_BEFORE_PTHREAD_ONCE, &ev, &md);

    ev.ret = ev.func(ev.once_control, ev.init_routine);

    struct after_pthread_once_event after_ev = {
        .pc = ev.pc,
        .once_control = ev.once_control,
        .init_routine = ev.init_routine,
        .ret = ev.ret,
        .func = ev.func,
    };
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_AFTER_PTHREAD_ONCE, &after_ev, &md);

    //PS_PUBLISH(INTERCEPT_AFTER, EVENT_PTHREAD_ONCE, &ev, &md);
    return after_ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_BEFORE_PTHREAD_ONCE)
PS_ADVERTISE_TYPE(EVENT_AFTER_PTHREAD_ONCE)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
