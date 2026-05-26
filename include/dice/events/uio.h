/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_UIO_H
#define DICE_UIO_H

#include <sys/uio.h>

#define EVENT_READV   136
#define EVENT_WRITEV  137

struct readv_event {
    const void *pc;
    int fd;
    const struct iovec *iov;
    int iovcnt;
    ssize_t ret;
    ssize_t(*func)(int, const struct iovec *, int);
};

struct writev_event {
    const void *pc;
    int fd;
    const struct iovec *iov;
    int iovcnt;
    ssize_t ret;
    ssize_t(*func)(int, const struct iovec *, int);
};

#endif /* DICE_UIO_H */
