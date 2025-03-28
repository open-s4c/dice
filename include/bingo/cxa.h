/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_CXA_H
#define BINGO_CXA_H

#include <bingo/thread_id.h>

enum cxa_events {
    EVENT_CXA_GUARD_ACQUIRE = 801,
    EVENT_CXA_GUARD_RELEASE = 802,
    EVENT_CXA_GUARD_ABORT   = 803,
};

struct cxa_event {
    void *addr;
    int ret;
};

#endif /* BINGO_CXA_H */
