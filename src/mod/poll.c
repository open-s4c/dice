/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/poll.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(int, poll, struct pollfd *fds, nfds_t nfds, int timeout)
{
    struct poll_event ev = {
        .pc      = INTERPOSE_PC,
        .fds     = fds,
        .nfds    = nfds,
        .timeout = timeout,
        .ret     = 0,
        .func    = REAL_FUNC(poll),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POLL, &ev, &md);
    ev.ret = ev.func(fds, nfds, timeout);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POLL, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_POLL)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
