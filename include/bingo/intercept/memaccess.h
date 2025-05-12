/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_MEMACCESS_H
#define BINGO_MEMACCESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <bingo/intercept.h>

#define EVENT_MA_READ         30
#define EVENT_MA_WRITE        31
#define EVENT_MA_AREAD        32
#define EVENT_MA_AWRITE       33
#define EVENT_MA_RMW          34
#define EVENT_MA_XCHG         35
#define EVENT_MA_CMPXCHG      36
#define EVENT_MA_CMPXCHG_WEAK 37
#define EVENT_MA_FENCE        38

typedef struct memaccess {
    const void *pc;
    const char *func;
    uintptr_t addr;
    size_t size;
    union {
        uint8_t argu8;
        uint16_t argu16;
        uint32_t argu32;
        uint64_t argu64;
        __uint128_t argu128;
    };
    bool failed; // optional argument to indicate operation failed
} memaccess_t;

#endif /* BINGO_MEMACCESS_H */
