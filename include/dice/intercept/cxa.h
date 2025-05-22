/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_CXA_H
#define DICE_CXA_H

#include <dice/intercept.h>

#define EVENT_CXA_GUARD_ACQUIRE 60
#define EVENT_CXA_GUARD_RELEASE 61
#define EVENT_CXA_GUARD_ABORT   62

struct cxa_event {
    const void *pc;
    void *addr;
    int ret;
};

#endif /* DICE_CXA_H */
