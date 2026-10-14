/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_STACKTRACE_H
#define DICE_STACKTRACE_H

#ifndef DICE_ROLLED
    #define EVENT_STACKTRACE_ENTER 41
    #define EVENT_STACKTRACE_EXIT  42
#endif

struct stacktrace_event {
    const void *pc;
    const void *caller;
    const char *fname;
};

typedef struct stacktrace_event stacktrace_event_t;

#endif /* DICE_STACKTRACE_H */
