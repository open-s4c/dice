/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_STACKTRACE_H
#define DICE_STACKTRACE_H

#define EVENT_STACKTRACE_ENTER 40
#define EVENT_STACKTRACE_EXIT  41

typedef struct {
    const void *pc;
    const void *caller;
    const char *fname;
} stacktrace_event_t;

#endif /* DICE_STACKTRACE_H */
