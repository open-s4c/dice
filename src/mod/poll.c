/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/poll.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <errno.h>

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
    errno = 0;
    ev.ret = ev.func(ev.fds, ev.nfds, ev.timeout);
    ev.errno_ = errno;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POLL, &ev, &md);
    errno = ev.errno_;

    return ev.ret;
}

INTERPOSE(int, ppoll, struct pollfd *fds, nfds_t nfds,
          const struct timespec *tmo_p, const sigset_t *sigmask)
{
    struct ppoll_event ev = {
        .pc      = INTERPOSE_PC,
        .fds     = fds,
        .nfds    = nfds,
        .tmo_p   = tmo_p,
        .sigmask = sigmask,
        .ret     = 0,
        .func    = REAL_FUNC(ppoll),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_PPOLL, &ev, &md);
    errno = 0;
    ev.ret = ev.func(ev.fds, ev.nfds, ev.tmo_p, ev.sigmask);
    ev.errno_ = errno;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_PPOLL, &ev, &md);
    errno = ev.errno_;

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_POLL)
PS_ADVERTISE_TYPE(EVENT_PPOLL)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
