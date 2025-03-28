/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_SEM_H
#define BINGO_SEM_H

enum sem_events {
    EVENT_SEM_POST      = 600,
    EVENT_SEM_WAIT      = 601,
    EVENT_SEM_TRYWAIT   = 602,
    EVENT_SEM_TIMEDWAIT = 603,
};

struct sem_event {
    void *sem;
    int ret;
};

#endif /* BINGO_SEM_H */
