/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_FCNTL_H
#define DICE_FCNTL_H

#include <fcntl.h>
#include <inttypes.h>

#define EVENT_CREAT           129
#define EVENT_FCNTL           130
#define EVENT_OPEN            131
#define EVENT_OPENAT          132
#define EVENT_OPENAT2         133
#define EVENT_POSIX_FADVISE   134
#define EVENT_POSIX_FALLOCATE 135

struct creat_event {
    const void *pc;
    const char *path;
    mode_t mode;
    int ret;
    int(*func)(const char *, mode_t);
};

struct fcntl_event {
    const void *pc;
    int fd;
    int cmd;
    uint64_t arg;
    int ret;
    int(*func)(int, int, ...);
};

struct open_event {
    const void *pc;
    const char *path;
    int flags;
    mode_t mode;
    int ret;
    int(*func)(const char *, int, ...);
};

struct openat_event {
    const void *pc;
    int dirfd;
    const char *path;
    int flags;
    mode_t mode;
    int ret;
    int(*func)(int, const char *, int, ...);
};

struct openat2_event {
    const void *pc;
    int dirfd;
    const char *path;
    const struct open_how *how;
    size_t size;
    int ret;
    int(*func)(int, const char *, const struct open_how *, size_t);
};

struct posix_fadvise_event {
    const void *pc;
    int fd;
    off_t offset;
    off_t size;
    int advice;
    int ret;
    int(*func)(int, off_t, off_t, int);
};

struct posix_fallocate_event {
    const void *pc;
    int fd;
    off_t offset;
    off_t size;
    int ret;
    int(*func)(int, off_t, off_t);
};

#endif /* DICE_FCNTL_H */