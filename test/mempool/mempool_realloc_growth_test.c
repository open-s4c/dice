/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <dice/mempool.h>

static void
fill_pattern(uint8_t *buf, size_t len, uint8_t base)
{
    for (size_t i = 0; i < len; ++i)
        buf[i] = base + (uint8_t)i;
}

static void
check_pattern(const uint8_t *buf, size_t len, uint8_t base)
{
    for (size_t i = 0; i < len; ++i)
        assert(buf[i] == (uint8_t)(base + i));
}

int
main(void)
{
    /* Seed the pool with an allocation that will be recycled. */
    uint8_t *seed = mempool_alloc(16);
    assert(seed != NULL);
    fill_pattern(seed, 16, 0x10);
    mempool_free(seed);

    /* Allocate a larger block from the same bucket and populate it fully. */
    const size_t original = 24;
    uint8_t *block        = mempool_alloc(original);
    assert(block != NULL);
    fill_pattern(block, original, 0x40);
    check_pattern(block, original, 0x40);

    /* Grow the block and verify the original payload is preserved. */
    const size_t grown = 48;
    uint8_t *bigger    = mempool_realloc(block, grown);
    assert(bigger != NULL);
    check_pattern(bigger, original, 0x40);

    mempool_free(bigger);
    return 0;
}
