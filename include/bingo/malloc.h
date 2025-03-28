/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MALLOC_H
#define BINGO_MALLOC_H

enum malloc_events {
    EVENT_MALLOC         = 700,
    EVENT_CALLOC         = 701,
    EVENT_REALLOC        = 702,
    EVENT_FREE           = 703,
    EVENT_POSIX_MEMALIGN = 704,
    EVENT_ALIGNED_ALLOC  = 705,
};

struct malloc_event {
    size_t size;
    void *addr;
    int ret;
};

#endif /* BINGO_MALLOC_H */
