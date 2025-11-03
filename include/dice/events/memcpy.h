/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_MEMCPY_H
#define DICE_MEMCPY_H

#include <stddef.h>
#include <string.h>

#define EVENT_MEMCPY  100
#define EVENT_MEMMOVE 101
#define EVENT_MEMSET  102

struct memcpy_event {
    const void *pc;
    void *dest;
    const void *src;
    size_t num;
    void *ret;
    void *(*func)(void *, const void *, size_t);
};

struct memmove_event {
    const void *pc;
    void *dest;
    const void *src;
    size_t count;
    void *ret;
    void *(*func)(void *, const void *, size_t);
};

struct memset_event {
    const void *pc;
    void *ptr;
    int value;
    size_t num;
    void *ret;
    void *(*func)(void *, int, size_t);
};

#endif /* DICE_MEMCPY_H */
