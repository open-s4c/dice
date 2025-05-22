/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_SEM_H
#define DICE_SEM_H

#include <dice/intercept.h>

#define EVENT_SEM_POST      70
#define EVENT_SEM_WAIT      71
#define EVENT_SEM_TRYWAIT   72
#define EVENT_SEM_TIMEDWAIT 73

struct sem_event {
    const void *pc;
    void *sem;
    int ret;
};

#endif /* DICE_SEM_H */
