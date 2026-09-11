/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/uio.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <errno.h>

INTERPOSE(ssize_t, readv, int fd, const struct iovec *iov, int iovcnt)
{
    struct readv_event ev = {
        .pc     = INTERPOSE_PC,
        .fd     = fd,
        .iov    = iov,
        .iovcnt = iovcnt,
        .ret    = 0,
        .func   = REAL_FUNC(readv),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READV, &ev, &md);

    errno = 0;
    ev.ret = ev.func(ev.fd, ev.iov, ev.iovcnt);
    ev.errno_ = errno;

    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READV, &ev, &md);
    errno = ev.errno_;

    return ev.ret;
}

INTERPOSE(ssize_t, writev, int fd, const struct iovec *iov, int iovcnt)
{
    struct writev_event ev = {
        .pc     = INTERPOSE_PC,
        .fd     = fd,
        .iov    = iov,
        .iovcnt = iovcnt,
        .ret    = 0,
        .func   = REAL_FUNC(writev),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_WRITEV, &ev, &md);

    errno = 0;
    ev.ret = ev.func(ev.fd, ev.iov, ev.iovcnt);
    ev.errno_ = errno;

    PS_PUBLISH(INTERCEPT_AFTER, EVENT_WRITEV, &ev, &md);
    errno = ev.errno_;

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_READV)
PS_ADVERTISE_TYPE(EVENT_WRITEV)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
