/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_CXA_H
#define BINGO_CXA_H

#include <bingo/intercept.h>

#define EVENT_CXA_GUARD_ACQUIRE 60
#define EVENT_CXA_GUARD_RELEASE 61
#define EVENT_CXA_GUARD_ABORT   62

struct cxa_event {
    const void *pc;
    void *addr;
    int ret;
};

#endif /* BINGO_CXA_H */
