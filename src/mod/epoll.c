/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/epoll.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(int, epoll_create, int size)
{
    struct epoll_create_event ev = {
        .pc   = INTERPOSE_PC,
        .size = size,
        .ret  = 0,
        .func = REAL_FUNC(epoll_create),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_CREATE, &ev, &md);
    ev.ret = ev.func(ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_CREATE, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, epoll_create1, int flags)
{
    struct epoll_create1_event ev = {
        .pc    = INTERPOSE_PC,
        .flags = flags,
        .ret   = 0,
        .func  = REAL_FUNC(epoll_create1),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_CREATE1, &ev, &md);
    ev.ret = ev.func(ev.flags);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_CREATE1, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, epoll_ctl, int epfd, int op, int fd, struct epoll_event *event)
{
    struct epoll_ctl_event ev = {
        .pc     = INTERPOSE_PC,
        .epfd   = epfd,
        .op     = op,
        .fd     = fd,
        .event  = event,
        .ret    = 0,
        .func   = REAL_FUNC(epoll_ctl),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_CTL, &ev, &md);
    ev.ret = ev.func(ev.epfd, ev.op, ev.fd, ev.event);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_CTL, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, epoll_wait, int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    struct epoll_wait_event ev = {
        .pc        = INTERPOSE_PC,
        .epfd      = epfd,
        .events    = events,
        .maxevents = maxevents,
        .timeout   = timeout,
        .ret       = 0,
        .func      = REAL_FUNC(epoll_wait),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_WAIT, &ev, &md);
    ev.ret = ev.func(ev.epfd, ev.events, ev.maxevents, ev.timeout);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_WAIT, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, epoll_pwait, int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
    struct epoll_pwait_event ev = {
        .pc        = INTERPOSE_PC,
        .epfd      = epfd,
        .events    = events,
        .maxevents = maxevents,
        .timeout   = timeout,
        .sigmask   = sigmask,
        .ret       = 0,
        .func      = REAL_FUNC(epoll_pwait),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_PWAIT, &ev, &md);
    ev.ret = ev.func(ev.epfd, ev.events, ev.maxevents, ev.timeout, ev.sigmask);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_PWAIT, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, epoll_pwait2, int epfd, struct epoll_event *events, int maxevents,
           const struct timespec *timeout, const sigset_t *sigmask)
{
    struct epoll_pwait2_event ev = {
        .pc        = INTERPOSE_PC,
        .epfd      = epfd,
        .events    = events,
        .maxevents = maxevents,
        .timeout   = timeout,
        .sigmask   = sigmask,
        .ret       = 0,
        .func      = REAL_FUNC(epoll_pwait2),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_EPOLL_PWAIT2, &ev, &md);
    ev.ret = ev.func(ev.epfd, ev.events, ev.maxevents, ev.timeout, ev.sigmask);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_EPOLL_PWAIT2, &ev, &md);

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_EPOLL_CREATE)
PS_ADVERTISE_TYPE(EVENT_EPOLL_CREATE1)
PS_ADVERTISE_TYPE(EVENT_EPOLL_CTL)
PS_ADVERTISE_TYPE(EVENT_EPOLL_WAIT)
PS_ADVERTISE_TYPE(EVENT_EPOLL_PWAIT)
PS_ADVERTISE_TYPE(EVENT_EPOLL_PWAIT2)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
