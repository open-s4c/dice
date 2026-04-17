/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_POLL_H
#define DICE_POLL_H

#include <poll.h>

#define EVENT_POLL   103

struct poll_event {
    const void *pc;
    struct pollfd *fds;
    nfds_t nfds;
    int timeout;
    int ret;
    int(*func)(struct pollfd*, nfds_t, int);
};

#endif /* DICE_POLL_H */
