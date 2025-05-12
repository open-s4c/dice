/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_STACKTRACE_H
#define BINGO_STACKTRACE_H

#include <bingo/intercept.h>

#define EVENT_STACKTRACE_ENTER 40
#define EVENT_STACKTRACE_EXIT  41

typedef struct {
    const void *pc;
    const void *caller;
    const char *fname;
} stacktrace_event_t;

#endif /* BINGO_STACKTRACE_H */
