/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_STRING_ALLOC_H
#define DICE_STRING_ALLOC_H

#include <stddef.h>
#include <wchar.h>

#if !defined(__NetBSD__)
    #define HAVE_WCSDUP
#endif

#define EVENT_STRDUP  56
#define EVENT_STRNDUP 57
#define EVENT_WCSDUP  58

struct strdup_event {
    const void *pc;
    const char *s;
    char *ret;
    char *(*func)(const char *);
};

struct strndup_event {
    const void *pc;
    const char *s;
    size_t n;
    char *ret;
    char *(*func)(const char *, size_t);
};

#ifdef HAVE_WCSDUP
struct wcsdup_event {
    const void *pc;
    const wchar_t *s;
    wchar_t *ret;
    wchar_t *(*func)(const wchar_t *);
};
#endif

#endif /* DICE_STRING_ALLOC_H */
