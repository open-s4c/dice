/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * @file tsan.c
 * @brief Implements a TSAN interface that publishes events.
 *
 ******************************************************************************/
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bingo/intercept/memaccess.h>
#include <bingo/interpose.h>
#include <bingo/module.h>

BINGO_MODULE_INIT()


/* empty tsan initialization */
void
__tsan_init()
{
}
void
__tsan_write_range(void)
{
}
void
__tsan_read_range(void)
{
}

/* empty vptr impl */
void
__tsan_vptr_read(void **vptr_p)
{
    (void)vptr_p;
}
void
__tsan_vptr_update(void **vptr_p, void *new_val)
{
    (void)vptr_p;
    (void)new_val;
}

/* with GCC version < 10, this symbols is defined */
void
internal_sigreturn(void)
{
}

void
__tsan_mutex_pre_lock(void *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
}
void
__tsan_mutex_post_lock(void *addr, unsigned flags, int recursion)
{
    (void)addr;
    (void)flags;
    (void)recursion;
}
int
__tsan_mutex_pre_unlock(void *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
    return 0;
}
void
__tsan_mutex_post_unlock(void *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
}
void
__tsan_mutex_create(void *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
}
void
__tsan_mutex_destroy(void *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
}
void
__tsan_acquire(void *addr)
{
    (void)addr;
}
void
__tsan_release(void *addr)
{
    (void)addr;
}
void *
__tsan_memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}
void *
__tsan_memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}
/* plain reads and writes */
void
__tsan_read1(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "read1",
        .addr    = (uintptr_t)a,
        .size    = 1,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_read2(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "read2",
        .addr    = (uintptr_t)a,
        .size    = 2,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_read4(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "read4",
        .addr    = (uintptr_t)a,
        .size    = 4,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_read8(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "read8",
        .addr    = (uintptr_t)a,
        .size    = 8,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_read16(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "read16",
        .addr    = (uintptr_t)a,
        .size    = 16,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_write1(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "write1",
        .addr    = (uintptr_t)a,
        .size    = 1,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_write2(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "write2",
        .addr    = (uintptr_t)a,
        .size    = 2,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_write4(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "write4",
        .addr    = (uintptr_t)a,
        .size    = 4,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_write8(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "write8",
        .addr    = (uintptr_t)a,
        .size    = 8,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_write16(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "write16",
        .addr    = (uintptr_t)a,
        .size    = 16,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_unaligned_read1(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_read1",
        .addr    = (uintptr_t)a,
        .size    = 1,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_unaligned_read2(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_read2",
        .addr    = (uintptr_t)a,
        .size    = 2,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_unaligned_read4(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_read4",
        .addr    = (uintptr_t)a,
        .size    = 4,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_unaligned_read8(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_read8",
        .addr    = (uintptr_t)a,
        .size    = 8,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_unaligned_read16(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_read16",
        .addr    = (uintptr_t)a,
        .size    = 16,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ma, 0);
}
void
__tsan_unaligned_write1(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_write1",
        .addr    = (uintptr_t)a,
        .size    = 1,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_unaligned_write2(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_write2",
        .addr    = (uintptr_t)a,
        .size    = 2,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_unaligned_write4(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_write4",
        .addr    = (uintptr_t)a,
        .size    = 4,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_unaligned_write8(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_write8",
        .addr    = (uintptr_t)a,
        .size    = 8,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}
void
__tsan_unaligned_write16(void *a)
{
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "unaligned_write16",
        .addr    = (uintptr_t)a,
        .size    = 16,
        .argu128 = 0,
        .failed  = false,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ma, 0);
}

/* plain reads and writes 2 values */
void
__tsan_read1_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, 0, 0);
}
void
__tsan_read2_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, 0, 0);
}
void
__tsan_read4_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, 0, 0);
}
void
__tsan_read8_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, 0, 0);
}
void
__tsan_read16_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, 0, 0);
}
void
__tsan_write1_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, 0, 0);
}
void
__tsan_write2_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, 0, 0);
}
void
__tsan_write4_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, 0, 0);
}
void
__tsan_write8_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, 0, 0);
}
void
__tsan_write16_pc(void *a, void *b)
{
    (void)a;
    (void)b;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, 0, 0);
}

/* atomic loads */
uint8_t
__tsan_atomic8_load(const volatile uint8_t *a, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic8_load",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = 0,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ma, &md);
    uint8_t r = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_load(const volatile uint16_t *a, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic16_load",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = 0,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ma, &md);
    uint16_t r = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_load(const volatile uint32_t *a, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic32_load",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = 0,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ma, &md);
    uint32_t r = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_load(const volatile uint64_t *a, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic64_load",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = 0,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ma, &md);
    uint64_t r = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ma, &md);
    return r;
}

/* atomic stores */
void
__tsan_atomic8_store(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_store",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ma, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ma, &md);
}
void
__tsan_atomic16_store(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_store",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ma, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ma, &md);
}
void
__tsan_atomic32_store(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_store",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ma, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ma, &md);
}
void
__tsan_atomic64_store(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_store",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ma, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ma, &md);
}


/* xchg */
uint8_t
__tsan_atomic8_exchange(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_exchange",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ma, &md);
    uint8_t r = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_exchange(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_exchange",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ma, &md);
    uint16_t r = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_exchange(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_exchange",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ma, &md);
    uint32_t r = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_exchange(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_exchange",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ma, &md);
    uint64_t r = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ma, &md);
    return r;
}

/* fetch_RMW */
uint8_t
__tsan_atomic8_fetch_add(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_add",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_add(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_add",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_add(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_add",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_add(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_add",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint8_t
__tsan_atomic8_fetch_sub(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_sub",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_sub(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_sub",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_sub(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_sub",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_sub(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_sub",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint8_t
__tsan_atomic8_fetch_and(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_and",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_and(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_and",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_and(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_and",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_and(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_and",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint8_t
__tsan_atomic8_fetch_or(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_or",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_or(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_or",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_or(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_or",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_or(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_or",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint8_t
__tsan_atomic8_fetch_xor(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_xor",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_xor(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_xor",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_xor(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_xor",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_xor(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_xor",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint8_t
__tsan_atomic8_fetch_nand(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_fetch_nand",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint8_t r = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint16_t
__tsan_atomic16_fetch_nand(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_fetch_nand",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint16_t r = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint32_t
__tsan_atomic32_fetch_nand(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_fetch_nand",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint32_t r = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}
uint64_t
__tsan_atomic64_fetch_nand(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_fetch_nand",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ma, &md);
    uint64_t r = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    ma.argu128   = (__uint128_t)r;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ma, &md);
    return r;
}


/* compare_exchange_{strong,weak} */
int
__tsan_atomic8_compare_exchange_strong(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_compare_exchange_strong",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic16_compare_exchange_strong(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_compare_exchange_strong",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic32_compare_exchange_strong(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_compare_exchange_strong",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic64_compare_exchange_strong(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_compare_exchange_strong",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic8_compare_exchange_weak(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_compare_exchange_weak",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic16_compare_exchange_weak(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_compare_exchange_weak",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic32_compare_exchange_weak(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_compare_exchange_weak",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}
int
__tsan_atomic64_compare_exchange_weak(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_compare_exchange_weak",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return r;
}

/* compare_exchange_val */
uint8_t
__tsan_atomic8_compare_exchange_val(volatile uint8_t *a, uint8_t c,
                                       uint8_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic8_compare_exchange_val",
        .addr    = (uintptr_t)a,
        .size    = (8 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return c;
}
uint16_t
__tsan_atomic16_compare_exchange_val(volatile uint16_t *a, uint16_t c,
                                       uint16_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic16_compare_exchange_val",
        .addr    = (uintptr_t)a,
        .size    = (16 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return c;
}
uint32_t
__tsan_atomic32_compare_exchange_val(volatile uint32_t *a, uint32_t c,
                                       uint32_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic32_compare_exchange_val",
        .addr    = (uintptr_t)a,
        .size    = (32 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return c;
}
uint64_t
__tsan_atomic64_compare_exchange_val(volatile uint64_t *a, uint64_t c,
                                       uint64_t v, int mo)
{
    (void)mo;
    memaccess_t ma = {
        .pc      = INTERPOSE_PC,
        .func    = "atomic64_compare_exchange_val",
        .addr    = (uintptr_t)a,
        .size    = (64 >> 3),
        .argu128 = (__uint128_t)v,
        .failed  = false,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ma, &md);
    int r = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    assert(r == 0 || r == 1);
    ma.failed = r == 0;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ma, &md);
    return c;
}

/* atomic fences */
void
__tsan_atomic_thread_fence(int mo)
{
    (void)mo;

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_FENCE, 0, &md);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_FENCE, 0, &md);
}

void
__tsan_atomic_signal_fence(int mo)
{
    (void)mo;

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_FENCE, 0, &md);
    __atomic_signal_fence(__ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_FENCE, 0, &md);
}
