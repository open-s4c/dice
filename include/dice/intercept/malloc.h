/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_MALLOC_H
#define DICE_MALLOC_H

#include <dice/intercept.h>

#define EVENT_MALLOC         50
#define EVENT_CALLOC         51
#define EVENT_REALLOC        52
#define EVENT_FREE           53
#define EVENT_POSIX_MEMALIGN 54
#define EVENT_ALIGNED_ALLOC  55

struct malloc_event {
    const void *pc;
    size_t size;
    void *ptr;
    int ret;
};

#endif /* DICE_MALLOC_H */
