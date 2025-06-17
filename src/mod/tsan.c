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

#include <dice/intercept/memaccess.h>
#include <dice/interpose.h>
#include <dice/module.h>

DICE_MODULE_INIT()


/* -----------------------------------------------------------------------------
 * empty tsan functions
 * -------------------------------------------------------------------------- */
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

/* -----------------------------------------------------------------------------
 * plain reads and writes
 * -------------------------------------------------------------------------- */
void
__tsan_read1(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "read1",
        .addr = (void *)a,
        .size = 1,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_read2(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "read2",
        .addr = (void *)a,
        .size = 2,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_read4(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "read4",
        .addr = (void *)a,
        .size = 4,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_read8(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "read8",
        .addr = (void *)a,
        .size = 8,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_read16(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "read16",
        .addr = (void *)a,
        .size = 16,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_write1(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "write1",
        .addr = (void *)a,
        .size = 1,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_write2(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "write2",
        .addr = (void *)a,
        .size = 2,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_write4(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "write4",
        .addr = (void *)a,
        .size = 4,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_write8(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "write8",
        .addr = (void *)a,
        .size = 8,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_write16(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "write16",
        .addr = (void *)a,
        .size = 16,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_unaligned_read1(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_read1",
        .addr = (void *)a,
        .size = 1,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_unaligned_read2(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_read2",
        .addr = (void *)a,
        .size = 2,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_unaligned_read4(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_read4",
        .addr = (void *)a,
        .size = 4,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_unaligned_read8(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_read8",
        .addr = (void *)a,
        .size = 8,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_unaligned_read16(void *a)
{
    struct ma_read_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_read16",
        .addr = (void *)a,
        .size = 16,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_READ, &ev, 0);
}
void
__tsan_unaligned_write1(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_write1",
        .addr = (void *)a,
        .size = 1,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_unaligned_write2(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_write2",
        .addr = (void *)a,
        .size = 2,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_unaligned_write4(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_write4",
        .addr = (void *)a,
        .size = 4,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_unaligned_write8(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_write8",
        .addr = (void *)a,
        .size = 8,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}
void
__tsan_unaligned_write16(void *a)
{
    struct ma_write_event ev = {
        .pc   = (INTERPOSE_PC),
        .func = "unaligned_write16",
        .addr = (void *)a,
        .size = 16,
    };

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_WRITE, &ev, 0);
}

/* -----------------------------------------------------------------------------
 * plain reads and writes 2 values
 * -------------------------------------------------------------------------- */
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

/* -----------------------------------------------------------------------------
 * atomic loads
 * -------------------------------------------------------------------------- */
uint8_t
__tsan_atomic8_load(const volatile uint8_t *a, int mo)
{
    struct ma_aread_event ev = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic8_load",
        .addr    = (void *)a,
        .size    = (8 >> 3),
        .mo      = mo,
        .val.u64 = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ev, &md);
    ev.val.u8 = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ev, &md);
    return ev.val.u8;
}
uint16_t
__tsan_atomic16_load(const volatile uint16_t *a, int mo)
{
    struct ma_aread_event ev = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic16_load",
        .addr    = (void *)a,
        .size    = (16 >> 3),
        .mo      = mo,
        .val.u64 = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ev, &md);
    ev.val.u16 = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ev, &md);
    return ev.val.u16;
}
uint32_t
__tsan_atomic32_load(const volatile uint32_t *a, int mo)
{
    struct ma_aread_event ev = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic32_load",
        .addr    = (void *)a,
        .size    = (32 >> 3),
        .mo      = mo,
        .val.u64 = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ev, &md);
    ev.val.u32 = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ev, &md);
    return ev.val.u32;
}
uint64_t
__tsan_atomic64_load(const volatile uint64_t *a, int mo)
{
    struct ma_aread_event ev = {
        .pc      = (INTERPOSE_PC),
        .func    = "atomic64_load",
        .addr    = (void *)a,
        .size    = (64 >> 3),
        .mo      = mo,
        .val.u64 = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AREAD, &ev, &md);
    ev.val.u64 = __atomic_load_n(a, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AREAD, &ev, &md);
    return ev.val.u64;
}

/* -----------------------------------------------------------------------------
 * atomic stores
 * -------------------------------------------------------------------------- */
void
__tsan_atomic8_store(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_awrite_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_store",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ev, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ev, &md);
}
void
__tsan_atomic16_store(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_awrite_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_store",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ev, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ev, &md);
}
void
__tsan_atomic32_store(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_awrite_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_store",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ev, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ev, &md);
}
void
__tsan_atomic64_store(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_awrite_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_store",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_AWRITE, &ev, &md);
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_AWRITE, &ev, &md);
}

/* -----------------------------------------------------------------------------
 * xchg
 * -------------------------------------------------------------------------- */
uint8_t
__tsan_atomic8_exchange(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_xchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_exchange",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ev, &md);
    ev.old.u8 = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_exchange(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_xchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_exchange",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ev, &md);
    ev.old.u16 = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_exchange(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_xchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_exchange",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ev, &md);
    ev.old.u32 = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_exchange(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_xchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_exchange",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_XCHG, &ev, &md);
    ev.old.u64 = __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_XCHG, &ev, &md);
    return ev.old.u64;
}

/* -----------------------------------------------------------------------------
 * fetch_RMW
 * -------------------------------------------------------------------------- */
uint8_t
__tsan_atomic8_fetch_add(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_add",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_add(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_add",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_add(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_add",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_add(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_add",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}
uint8_t
__tsan_atomic8_fetch_sub(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_sub",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_sub(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_sub",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_sub(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_sub",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_sub(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_sub",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}
uint8_t
__tsan_atomic8_fetch_and(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_and",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_and(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_and",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_and(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_and",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_and(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_and",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}
uint8_t
__tsan_atomic8_fetch_or(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_or",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_or(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_or",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_or(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_or",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_or(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_or",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}
uint8_t
__tsan_atomic8_fetch_xor(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_xor",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_xor(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_xor",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_xor(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_xor",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_xor(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_xor",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}
uint8_t
__tsan_atomic8_fetch_nand(volatile uint8_t *a, uint8_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_fetch_nand",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u8 = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_fetch_nand(volatile uint16_t *a, uint16_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_fetch_nand",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u16 = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_fetch_nand(volatile uint32_t *a, uint32_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_fetch_nand",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u32 = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_fetch_nand(volatile uint64_t *a, uint64_t v, int mo)
{
    struct ma_rmw_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_fetch_nand",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .old.u64   = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_RMW, &ev, &md);
    ev.old.u64 = __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_RMW, &ev, &md);
    return ev.old.u64;
}

/* -----------------------------------------------------------------------------
 * compare_exchange_{strong,weak}
 * -------------------------------------------------------------------------- */
int
__tsan_atomic8_compare_exchange_strong(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_compare_exchange_strong",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .cmp.u8 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u8 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic16_compare_exchange_strong(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_compare_exchange_strong",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .cmp.u16 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u16 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic32_compare_exchange_strong(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_compare_exchange_strong",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .cmp.u32 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u32 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic64_compare_exchange_strong(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_compare_exchange_strong",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .cmp.u64 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u64 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic8_compare_exchange_weak(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_compare_exchange_weak",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .cmp.u8 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u8 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic16_compare_exchange_weak(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_compare_exchange_weak",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .cmp.u16 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u16 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic32_compare_exchange_weak(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_compare_exchange_weak",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .cmp.u32 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u32 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}
int
__tsan_atomic64_compare_exchange_weak(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_compare_exchange_weak",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .cmp.u64 = *c,
        .old.u64   = 0,
        .ok        = 0,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok = __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST);
    ev.old.u64 = *c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.ok;
}

/* -----------------------------------------------------------------------------
 * compare_exchange_val
 * -------------------------------------------------------------------------- */
uint8_t
__tsan_atomic8_compare_exchange_val(volatile uint8_t *a, uint8_t c,
                                       uint8_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic8_compare_exchange_val",
        .addr      = (void *)a,
        .size      = (8 >> 3),
        .mo        = mo,
        .val.u8 = v,
        .cmp.u8 = c,
        .old.u64   = 0,
        .ok        = 0,
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok        = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST);
    ev.old.u8 = c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.old.u8;
}
uint16_t
__tsan_atomic16_compare_exchange_val(volatile uint16_t *a, uint16_t c,
                                       uint16_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic16_compare_exchange_val",
        .addr      = (void *)a,
        .size      = (16 >> 3),
        .mo        = mo,
        .val.u16 = v,
        .cmp.u16 = c,
        .old.u64   = 0,
        .ok        = 0,
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok        = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST);
    ev.old.u16 = c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.old.u16;
}
uint32_t
__tsan_atomic32_compare_exchange_val(volatile uint32_t *a, uint32_t c,
                                       uint32_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic32_compare_exchange_val",
        .addr      = (void *)a,
        .size      = (32 >> 3),
        .mo        = mo,
        .val.u32 = v,
        .cmp.u32 = c,
        .old.u64   = 0,
        .ok        = 0,
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok        = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST);
    ev.old.u32 = c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.old.u32;
}
uint64_t
__tsan_atomic64_compare_exchange_val(volatile uint64_t *a, uint64_t c,
                                       uint64_t v, int mo)
{
    struct ma_cmpxchg_event ev = {
        .pc        = INTERPOSE_PC,
        .func      = "atomic64_compare_exchange_val",
        .addr      = (void *)a,
        .size      = (64 >> 3),
        .mo        = mo,
        .val.u64 = v,
        .cmp.u64 = c,
        .old.u64   = 0,
        .ok        = 0,
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, &ev, &md);
    ev.ok        = __atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                               __ATOMIC_SEQ_CST);
    ev.old.u64 = c;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, &ev, &md);
    return ev.old.u64;
}

/* -----------------------------------------------------------------------------
 * atomic fences
 * -------------------------------------------------------------------------- */
void
__tsan_atomic_thread_fence(int mo)
{
    struct ma_fence_event ev = {
        .pc   = INTERPOSE_PC,
        .func = "atomic_thread_fence",
        .mo   = mo,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_FENCE, &ev, &md);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_FENCE, &ev, &md);
}

void
__tsan_atomic_signal_fence(int mo)
{
    struct ma_fence_event ev = {
        .pc   = INTERPOSE_PC,
        .func = "atomic_signal_fence",
        .mo   = mo,
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MA_FENCE, &ev, &md);
    __atomic_signal_fence(__ATOMIC_SEQ_CST);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MA_FENCE, &ev, &md);
}
