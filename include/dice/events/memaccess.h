/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_MEMACCESS_H
#define DICE_MEMACCESS_H

#include <stddef.h>
#include <stdint.h>

#define EVENT_MA_READ         30
#define EVENT_MA_WRITE        31
#define EVENT_MA_AREAD        32
#define EVENT_MA_AWRITE       33
#define EVENT_MA_RMW          34
#define EVENT_MA_XCHG         35
#define EVENT_MA_CMPXCHG      36
#define EVENT_MA_CMPXCHG_WEAK 37
#define EVENT_MA_FENCE        38

struct ma_read_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
};

struct ma_write_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
};

struct ma_aread_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
    int mo;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } val;
};

struct ma_awrite_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
    int mo;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } val;
};

struct ma_xchg_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
    int mo;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } val;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } old;
};

struct ma_rmw_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
    int mo;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } val;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } old;
};

struct ma_cmpxchg_event {
    const void *pc;
    const char *func;
    void *addr;
    size_t size;
    int mo;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } val;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } cmp;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } old;
    int ok;
};

struct ma_fence_event {
    const void *pc;
    const char *func;
    int mo;
};

#endif /* DICE_MEMACCESS_H */
