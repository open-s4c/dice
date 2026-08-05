/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_POLL_H
#define DICE_POLL_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <poll.h>
#include <signal.h>

#define EVENT_POLL 103
#define EVENT_PPOLL 263

struct poll_event {
    const void *pc;
    struct pollfd *fds;
    nfds_t nfds;
    int timeout;
    int ret;
    int errno_;
    int(*func)(struct pollfd*, nfds_t, int);
};

struct ppoll_event {
    const void *pc;
    struct pollfd *fds;
    nfds_t nfds;
    const struct timespec *tmo_p;
    const sigset_t *sigmask;
    int ret;
    int errno_;
    int(*func)(struct pollfd*, nfds_t, const struct timespec*, const sigset_t*);
};

#endif /* DICE_POLL_H */
