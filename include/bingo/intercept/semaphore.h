/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_SEM_H
#define BINGO_SEM_H

#include <bingo/intercept.h>

#define EVENT_SEM_POST      70
#define EVENT_SEM_WAIT      71
#define EVENT_SEM_TRYWAIT   72
#define EVENT_SEM_TIMEDWAIT 73

struct sem_event {
    const void *pc;
    void *sem;
    int ret;
};

#endif /* BINGO_SEM_H */
