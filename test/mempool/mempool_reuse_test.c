/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <dice/mempool.h>

int
main(void)
{
    const size_t size = 64;
    uint8_t *first    = mempool_alloc(size);
    assert(first != NULL);

    memset(first, 0xAB, size);
    mempool_free(first);

    uint8_t *second = mempool_alloc(size);
    assert(second != NULL);
    assert(second == first);

    for (size_t i = 0; i < size; ++i)
        assert(second[i] == 0xAB);

    mempool_free(second);
    return 0;
}
