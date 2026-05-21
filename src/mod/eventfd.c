/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/eventfd.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(int, eventfd, unsigned int initval, int flags)
{
    struct eventfd_event ev = {
        .pc       = INTERPOSE_PC,
        .initval  = initval,
        .flags    = flags,
        .ret      = 0,
        .func     = REAL_FUNC(eventfd),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EVENTFD, &ev, &md);
    ev.ret = ev.func(ev.initval, ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EVENTFD, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_EVENTFD)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
