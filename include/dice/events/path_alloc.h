/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_PATH_ALLOC_H
#define DICE_PATH_ALLOC_H

#include <stddef.h>

#if !defined(__APPLE__) && !defined(__NetBSD__) && !defined(__OHOS__)
    #define HAVE_GET_CURRENT_DIR_NAME
#endif

#define EVENT_GET_CURRENT_DIR_NAME 63
#define EVENT_GETCWD               64
#define EVENT_REALPATH             65
#define EVENT_TEMPNAM              66

#ifdef HAVE_GET_CURRENT_DIR_NAME
struct get_current_dir_name_event {
    const void *pc;
    char *ret;
    char *(*func)(void);
};
#endif

struct getcwd_event {
    const void *pc;
    char *buf;
    size_t size;
    char *ret;
    char *(*func)(char *, size_t);
};

struct realpath_event {
    const void *pc;
    const char *path;
    char *resolved_path;
    char *ret;
    char *(*func)(const char *, char *);
};

struct tempnam_event {
    const void *pc;
    const char *dir;
    const char *pfx;
    char *ret;
    char *(*func)(const char *, const char *);
};

#endif /* DICE_PATH_ALLOC_H */
