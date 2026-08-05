/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_RANDOM_H
#define DICE_RANDOM_H

#include <sys/random.h>

#define EVENT_GETRANDOM   262

struct getrandom_event {
    const void *pc;
    void *buf;
    size_t size;
    unsigned int flags;
    ssize_t ret;
    ssize_t(*func)(void*, size_t, unsigned int);
};

#endif /* DICE_RANDOM_H */
