/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <dice/ensure.h>
#include <dice/mempool.h>

int
main(void)
{
    srand(time(NULL));
    while (1) {
        size_t alignment = 1 << (rand() % 20);
        size_t size = rand() % 1024 * 1024;
        void *ptr = mempool_aligned_alloc(alignment, size);

        if (ptr == NULL) {
            break;
        }

        ensure((uintptr_t)ptr % alignment == 0);
    }
}
