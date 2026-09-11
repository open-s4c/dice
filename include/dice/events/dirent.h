/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_DIRENT_H
#define DICE_DIRENT_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dirent.h>

#define EVENT_CLOSEDIR  267
#define EVENT_FDOPENDIR 268
#define EVENT_OPENDIR   269
#define EVENT_READDIR   270
#define EVENT_READDIR_R 271
#define EVENT_REWINDDIR 272
#define EVENT_SEEKDIR   273
#define EVENT_TELLDIR   274

struct closedir_event {
    const void *pc;
    DIR *dirp;
    int ret;
    int(*func)(DIR *);
};

struct fdopendir_event {
    const void *pc;
    int fd;
    DIR *ret;
    DIR *(*func)(int);
};

struct opendir_event {
    const void *pc;
    const char *name;
    DIR *ret;
    DIR *(*func)(const char *);
};

struct readdir_event {
    const void *pc;
    DIR *dirp;
    struct dirent *ret;
    struct dirent *(*func)(DIR *);
};

struct readdir_r_event {
    const void *pc;
    DIR *dirp;
    struct dirent *entry;
    struct dirent **result;
    int ret;
    int(*func)(DIR *, struct dirent *, struct dirent **);
};

struct rewinddir_event {
    const void *pc;
    DIR *dirp;
    void(*func)(DIR *);
};

struct seekdir_event {
    const void *pc;
    DIR *dirp;
    long loc;
    void(*func)(DIR *, long);
};

struct telldir_event {
    const void *pc;
    DIR *dirp;
    long ret;
    long(*func)(DIR *);
};

#endif /* DICE_DIRENT_H */
