/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/random.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>
#include <dice/module.h>

INTERPOSE(ssize_t, getrandom, void *buf, size_t size, unsigned int flags)
{
    struct getrandom_event ev = {
        .pc    = INTERPOSE_PC,
        .buf   = buf,
        .size  = size,
        .flags = flags,
        .ret   = 0,
        .func  = REAL_FUNC(getrandom),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETRANDOM, &ev, &md);
    ev.ret = ev.func(buf, size, flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETRANDOM, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_GETRANDOM)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
