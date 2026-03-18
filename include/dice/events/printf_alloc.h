/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_PRINTF_ALLOC_H
#define DICE_PRINTF_ALLOC_H

#include <stdarg.h>

#define EVENT_ASPRINTF  67
#define EVENT_VASPRINTF 68

struct asprintf_event {
    const void *pc;
    char **strp;
    const char *fmt;
    int ret;
    int (*func)(char **, const char *, ...);
    int (*vfunc)(char **, const char *, va_list);
};

struct vasprintf_event {
    const void *pc;
    char **strp;
    const char *fmt;
    va_list ap;
    int ret;
    int (*func)(char **, const char *, va_list);
};

#endif /* DICE_PRINTF_ALLOC_H */
