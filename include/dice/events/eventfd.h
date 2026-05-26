/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_EVENTFD_H
#define DICE_EVENTFD_H

#define EVENT_EVENTFD 128

struct eventfd_event {
    const void *pc;
    unsigned int initval;
    int flags;
    int ret;
    int(*func)(unsigned int, int);
};

#endif /* DICE_EVENTFD_H */
