/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dice/log.h"
#include <dice/ensure.h>
#include <dice/mempool.h>

#define N_ITERATIONS 100000
#define N_ALIGNMENTS 20
#define N_SLABS      9
#define N_ADDRESSES  (N_SLABS * N_ALIGNMENTS)

void* seen_addresses[N_ADDRESSES] = {};

static void _check_address(void *ptr)
{
    int index = 0;
    while (index < N_ADDRESSES && (seen_addresses[index] != ptr && seen_addresses[index] != NULL)) {
        index++;
    }
    ensure(index < N_ADDRESSES);
    seen_addresses[index] = ptr;
}

int
main(void)
{
    srand(time(NULL));
    for (int iteration = 0; iteration < N_ITERATIONS; iteration++) {
        size_t alignment = 1 << (rand() % N_ALIGNMENTS);
        size_t size = rand() % 1024 * 1024;
        char data = rand() % (1 << sizeof(char));
        void *ptr = mempool_aligned_alloc(alignment, size);

        if (ptr == NULL) {
            break;
        }

        ensure((uintptr_t)ptr % alignment == 0);
        _check_address(ptr);
        memset(ptr, data, size);
        mempool_free(ptr);
    }
}
