/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_PRINTF_ALLOC_H
#define DICE_PRINTF_ALLOC_H

#include <stdarg.h>

#if !defined(__NetBSD__)
    #define HAVE_ASPRINTF
    #define HAVE_VASPRINTF
#endif

#define EVENT_ASPRINTF  67
#define EVENT_VASPRINTF 68

#ifdef HAVE_ASPRINTF
struct asprintf_event {
    const void *pc;
    char **strp;
    const char *fmt;
    int ret;
    int (*func)(char **, const char *, ...);
    int (*vfunc)(char **, const char *, va_list);
};
#endif

#ifdef HAVE_VASPRINTF
struct vasprintf_event {
    const void *pc;
    char **strp;
    const char *fmt;
    va_list ap;
    int ret;
    int (*func)(char **, const char *, va_list);
};
#endif

#endif /* DICE_PRINTF_ALLOC_H */
