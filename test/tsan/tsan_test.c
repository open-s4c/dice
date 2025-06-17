/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <dice/intercept.h>
#include <dice/intercept/memaccess.h>
#include <dice/interpose.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/pubsub.h>


#define ensure(COND)                                                           \
    {                                                                          \
        if (!(COND)) {                                                         \
            log_fatalf("error: %s\n", #COND);                                  \
        }                                                                      \
    }

typedef void (*test_f)(void);

static size_t test_count;
static test_f tests[4096];

static void
init_event(void *ptr, size_t n)
{
    char *buf = ptr;
    for (size_t i = 0; i < n; i++)
        buf[i] = rand() % 256;
}

/* -----------------------------------------------------------------------------
 * plain reads
 * -------------------------------------------------------------------------- */
static struct ma_read_event _exp_read;

void __tsan_read1(void *a);
static void
test_read1(void)
{
    log_printf("Testing read1\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_read1(_exp_read.addr);
}
static DICE_CTOR void
register_read1(void)
{
    tests[test_count++] = test_read1;
}
void __tsan_read2(void *a);
static void
test_read2(void)
{
    log_printf("Testing read2\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_read2(_exp_read.addr);
}
static DICE_CTOR void
register_read2(void)
{
    tests[test_count++] = test_read2;
}
void __tsan_read4(void *a);
static void
test_read4(void)
{
    log_printf("Testing read4\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_read4(_exp_read.addr);
}
static DICE_CTOR void
register_read4(void)
{
    tests[test_count++] = test_read4;
}
void __tsan_read8(void *a);
static void
test_read8(void)
{
    log_printf("Testing read8\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_read8(_exp_read.addr);
}
static DICE_CTOR void
register_read8(void)
{
    tests[test_count++] = test_read8;
}
void __tsan_read16(void *a);
static void
test_read16(void)
{
    log_printf("Testing read16\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_read16(_exp_read.addr);
}
static DICE_CTOR void
register_read16(void)
{
    tests[test_count++] = test_read16;
}
void __tsan_unaligned_read1(void *a);
static void
test_unaligned_read1(void)
{
    log_printf("Testing unaligned_read1\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_unaligned_read1(_exp_read.addr);
}
static DICE_CTOR void
register_unaligned_read1(void)
{
    tests[test_count++] = test_unaligned_read1;
}
void __tsan_unaligned_read2(void *a);
static void
test_unaligned_read2(void)
{
    log_printf("Testing unaligned_read2\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_unaligned_read2(_exp_read.addr);
}
static DICE_CTOR void
register_unaligned_read2(void)
{
    tests[test_count++] = test_unaligned_read2;
}
void __tsan_unaligned_read4(void *a);
static void
test_unaligned_read4(void)
{
    log_printf("Testing unaligned_read4\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_unaligned_read4(_exp_read.addr);
}
static DICE_CTOR void
register_unaligned_read4(void)
{
    tests[test_count++] = test_unaligned_read4;
}
void __tsan_unaligned_read8(void *a);
static void
test_unaligned_read8(void)
{
    log_printf("Testing unaligned_read8\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_unaligned_read8(_exp_read.addr);
}
static DICE_CTOR void
register_unaligned_read8(void)
{
    tests[test_count++] = test_unaligned_read8;
}
void __tsan_unaligned_read16(void *a);
static void
test_unaligned_read16(void)
{
    log_printf("Testing unaligned_read16\n");
    init_event(&_exp_read, sizeof(struct ma_read_event));
    __tsan_unaligned_read16(_exp_read.addr);
}
static DICE_CTOR void
register_unaligned_read16(void)
{
    tests[test_count++] = test_unaligned_read16;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_MA_READ, {
    struct ma_read_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "read1") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 1);
    } else if (strcmp(ev->func, "read2") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 2);
    } else if (strcmp(ev->func, "read4") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 4);
    } else if (strcmp(ev->func, "read8") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 8);
    } else if (strcmp(ev->func, "read16") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 16);
    } else if (strcmp(ev->func, "unaligned_read1") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 1);
    } else if (strcmp(ev->func, "unaligned_read2") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 2);
    } else if (strcmp(ev->func, "unaligned_read4") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 4);
    } else if (strcmp(ev->func, "unaligned_read8") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 8);
    } else if (strcmp(ev->func, "unaligned_read16") == 0) {
        ensure(ev->addr == _exp_read.addr);
        ensure(ev->size == 16);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * plain writes
 * -------------------------------------------------------------------------- */
static struct ma_write_event _exp_write;

void __tsan_write1(void *a);
static void
test_write1(void)
{
    log_printf("Testing write1\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_write1(_exp_write.addr);
}
static DICE_CTOR void
register_write1(void)
{
    tests[test_count++] = test_write1;
}
void __tsan_write2(void *a);
static void
test_write2(void)
{
    log_printf("Testing write2\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_write2(_exp_write.addr);
}
static DICE_CTOR void
register_write2(void)
{
    tests[test_count++] = test_write2;
}
void __tsan_write4(void *a);
static void
test_write4(void)
{
    log_printf("Testing write4\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_write4(_exp_write.addr);
}
static DICE_CTOR void
register_write4(void)
{
    tests[test_count++] = test_write4;
}
void __tsan_write8(void *a);
static void
test_write8(void)
{
    log_printf("Testing write8\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_write8(_exp_write.addr);
}
static DICE_CTOR void
register_write8(void)
{
    tests[test_count++] = test_write8;
}
void __tsan_write16(void *a);
static void
test_write16(void)
{
    log_printf("Testing write16\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_write16(_exp_write.addr);
}
static DICE_CTOR void
register_write16(void)
{
    tests[test_count++] = test_write16;
}
void __tsan_unaligned_write1(void *a);
static void
test_unaligned_write1(void)
{
    log_printf("Testing unaligned_write1\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_unaligned_write1(_exp_write.addr);
}
static DICE_CTOR void
register_unaligned_write1(void)
{
    tests[test_count++] = test_unaligned_write1;
}
void __tsan_unaligned_write2(void *a);
static void
test_unaligned_write2(void)
{
    log_printf("Testing unaligned_write2\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_unaligned_write2(_exp_write.addr);
}
static DICE_CTOR void
register_unaligned_write2(void)
{
    tests[test_count++] = test_unaligned_write2;
}
void __tsan_unaligned_write4(void *a);
static void
test_unaligned_write4(void)
{
    log_printf("Testing unaligned_write4\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_unaligned_write4(_exp_write.addr);
}
static DICE_CTOR void
register_unaligned_write4(void)
{
    tests[test_count++] = test_unaligned_write4;
}
void __tsan_unaligned_write8(void *a);
static void
test_unaligned_write8(void)
{
    log_printf("Testing unaligned_write8\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_unaligned_write8(_exp_write.addr);
}
static DICE_CTOR void
register_unaligned_write8(void)
{
    tests[test_count++] = test_unaligned_write8;
}
void __tsan_unaligned_write16(void *a);
static void
test_unaligned_write16(void)
{
    log_printf("Testing unaligned_write16\n");
    init_event(&_exp_write, sizeof(struct ma_write_event));
    __tsan_unaligned_write16(_exp_write.addr);
}
static DICE_CTOR void
register_unaligned_write16(void)
{
    tests[test_count++] = test_unaligned_write16;
}

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_MA_WRITE, {
    struct ma_read_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "write1") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 1);
    } else if (strcmp(ev->func, "write2") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 2);
    } else if (strcmp(ev->func, "write4") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 4);
    } else if (strcmp(ev->func, "write8") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 8);
    } else if (strcmp(ev->func, "write16") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 16);
    } else if (strcmp(ev->func, "unaligned_write1") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 1);
    } else if (strcmp(ev->func, "unaligned_write2") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 2);
    } else if (strcmp(ev->func, "unaligned_write4") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 4);
    } else if (strcmp(ev->func, "unaligned_write8") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 8);
    } else if (strcmp(ev->func, "unaligned_write16") == 0) {
        ensure(ev->addr == _exp_write.addr);
        ensure(ev->size == 16);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * plain double reads and writes
 * -------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
 * atomic loads
 * -------------------------------------------------------------------------- */
static struct ma_aread_event _exp_aread;

uint8_t __tsan_atomic8_load(const volatile uint8_t *a, int mo);


static void
test_atomic8_load(void)
{
    log_printf("Testing atomic8_load\n");
    init_event(&_exp_aread, sizeof(struct ma_aread_event));
    uint8_t val     = _exp_aread.val.u8;
    _exp_aread.addr = (void *)&val;

    uint8_t ret = __tsan_atomic8_load(&val, _exp_aread.mo);
    ensure(ret == _exp_aread.val.u8);
}
static DICE_CTOR void
register_atomic8_load(void)
{
    tests[test_count++] = test_atomic8_load;
}
uint16_t __tsan_atomic16_load(const volatile uint16_t *a, int mo);


static void
test_atomic16_load(void)
{
    log_printf("Testing atomic16_load\n");
    init_event(&_exp_aread, sizeof(struct ma_aread_event));
    uint16_t val    = _exp_aread.val.u16;
    _exp_aread.addr = (void *)&val;

    uint16_t ret = __tsan_atomic16_load(&val, _exp_aread.mo);
    ensure(ret == _exp_aread.val.u16);
}
static DICE_CTOR void
register_atomic16_load(void)
{
    tests[test_count++] = test_atomic16_load;
}
uint32_t __tsan_atomic32_load(const volatile uint32_t *a, int mo);


static void
test_atomic32_load(void)
{
    log_printf("Testing atomic32_load\n");
    init_event(&_exp_aread, sizeof(struct ma_aread_event));
    uint32_t val    = _exp_aread.val.u32;
    _exp_aread.addr = (void *)&val;

    uint32_t ret = __tsan_atomic32_load(&val, _exp_aread.mo);
    ensure(ret == _exp_aread.val.u32);
}
static DICE_CTOR void
register_atomic32_load(void)
{
    tests[test_count++] = test_atomic32_load;
}
uint64_t __tsan_atomic64_load(const volatile uint64_t *a, int mo);


static void
test_atomic64_load(void)
{
    log_printf("Testing atomic64_load\n");
    init_event(&_exp_aread, sizeof(struct ma_aread_event));
    uint64_t val    = _exp_aread.val.u64;
    _exp_aread.addr = (void *)&val;

    uint64_t ret = __tsan_atomic64_load(&val, _exp_aread.mo);
    ensure(ret == _exp_aread.val.u64);
}
static DICE_CTOR void
register_atomic64_load(void)
{
    tests[test_count++] = test_atomic64_load;
}

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_AREAD, {
    struct ma_aread_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_aread.mo);
    } else if (strcmp(ev->func, "atomic16_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_aread.mo);
    } else if (strcmp(ev->func, "atomic32_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_aread.mo);
    } else if (strcmp(ev->func, "atomic64_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_aread.mo);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_AREAD, {
    struct ma_aread_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_aread.mo);
        ensure(ev->val.u8 == _exp_aread.val.u8);
    } else if (strcmp(ev->func, "atomic16_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_aread.mo);
        ensure(ev->val.u16 == _exp_aread.val.u16);
    } else if (strcmp(ev->func, "atomic32_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_aread.mo);
        ensure(ev->val.u32 == _exp_aread.val.u32);
    } else if (strcmp(ev->func, "atomic64_load") == 0) {
        ensure(ev->addr == _exp_aread.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_aread.mo);
        ensure(ev->val.u64 == _exp_aread.val.u64);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * atomic stores
 * -------------------------------------------------------------------------- */
static struct ma_awrite_event _exp_awrite;

void __tsan_atomic8_store(volatile uint8_t *a, uint8_t v, int mo);

static void
test_atomic8_store(void)
{
    log_printf("Testing atomic8_store\n");
    init_event(&_exp_awrite, sizeof(struct ma_awrite_event));
    uint8_t val      = ~_exp_awrite.val.u8;
    _exp_awrite.addr = (void *)&val;
    __tsan_atomic8_store(&val, _exp_awrite.val.u8, _exp_awrite.mo);
    ensure(val == _exp_awrite.val.u8);
}
static DICE_CTOR void
register_atomic8_store(void)
{
    tests[test_count++] = test_atomic8_store;
}
void __tsan_atomic16_store(volatile uint16_t *a, uint16_t v, int mo);

static void
test_atomic16_store(void)
{
    log_printf("Testing atomic16_store\n");
    init_event(&_exp_awrite, sizeof(struct ma_awrite_event));
    uint16_t val     = ~_exp_awrite.val.u16;
    _exp_awrite.addr = (void *)&val;
    __tsan_atomic16_store(&val, _exp_awrite.val.u16, _exp_awrite.mo);
    ensure(val == _exp_awrite.val.u16);
}
static DICE_CTOR void
register_atomic16_store(void)
{
    tests[test_count++] = test_atomic16_store;
}
void __tsan_atomic32_store(volatile uint32_t *a, uint32_t v, int mo);

static void
test_atomic32_store(void)
{
    log_printf("Testing atomic32_store\n");
    init_event(&_exp_awrite, sizeof(struct ma_awrite_event));
    uint32_t val     = ~_exp_awrite.val.u32;
    _exp_awrite.addr = (void *)&val;
    __tsan_atomic32_store(&val, _exp_awrite.val.u32, _exp_awrite.mo);
    ensure(val == _exp_awrite.val.u32);
}
static DICE_CTOR void
register_atomic32_store(void)
{
    tests[test_count++] = test_atomic32_store;
}
void __tsan_atomic64_store(volatile uint64_t *a, uint64_t v, int mo);

static void
test_atomic64_store(void)
{
    log_printf("Testing atomic64_store\n");
    init_event(&_exp_awrite, sizeof(struct ma_awrite_event));
    uint64_t val     = ~_exp_awrite.val.u64;
    _exp_awrite.addr = (void *)&val;
    __tsan_atomic64_store(&val, _exp_awrite.val.u64, _exp_awrite.mo);
    ensure(val == _exp_awrite.val.u64);
}
static DICE_CTOR void
register_atomic64_store(void)
{
    tests[test_count++] = test_atomic64_store;
}

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_AWRITE, {
    struct ma_awrite_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u8 == _exp_awrite.val.u8);
    } else if (strcmp(ev->func, "atomic16_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u16 == _exp_awrite.val.u16);
    } else if (strcmp(ev->func, "atomic32_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u32 == _exp_awrite.val.u32);
    } else if (strcmp(ev->func, "atomic64_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u64 == _exp_awrite.val.u64);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_AWRITE, {
    struct ma_awrite_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u8 == _exp_awrite.val.u8);
    } else if (strcmp(ev->func, "atomic16_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u16 == _exp_awrite.val.u16);
    } else if (strcmp(ev->func, "atomic32_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u32 == _exp_awrite.val.u32);
    } else if (strcmp(ev->func, "atomic64_store") == 0) {
        ensure(ev->addr == _exp_awrite.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_awrite.mo);
        ensure(ev->val.u64 == _exp_awrite.val.u64);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * xchg
 * -------------------------------------------------------------------------- */
struct ma_xchg_event _exp_xchg;

uint8_t __tsan_atomic8_exchange(volatile uint8_t *a, uint8_t v, int mo);

static void
test_atomic8_exchange(void)
{
    log_printf("Testing atomic8_exchange\n");
    init_event(&_exp_xchg, sizeof(struct ma_xchg_event));
    uint8_t val    = _exp_xchg.old.u8;
    _exp_xchg.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_exchange(&val, _exp_xchg.val.u8, _exp_xchg.mo);
    ensure(val == _exp_xchg.val.u8);
    ensure(ret == _exp_xchg.old.u8);
}
static DICE_CTOR void
register_atomic8_xchg(void)
{
    tests[test_count++] = test_atomic8_exchange;
}
uint16_t __tsan_atomic16_exchange(volatile uint16_t *a, uint16_t v, int mo);

static void
test_atomic16_exchange(void)
{
    log_printf("Testing atomic16_exchange\n");
    init_event(&_exp_xchg, sizeof(struct ma_xchg_event));
    uint16_t val   = _exp_xchg.old.u16;
    _exp_xchg.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_exchange(&val, _exp_xchg.val.u16, _exp_xchg.mo);
    ensure(val == _exp_xchg.val.u16);
    ensure(ret == _exp_xchg.old.u16);
}
static DICE_CTOR void
register_atomic16_xchg(void)
{
    tests[test_count++] = test_atomic16_exchange;
}
uint32_t __tsan_atomic32_exchange(volatile uint32_t *a, uint32_t v, int mo);

static void
test_atomic32_exchange(void)
{
    log_printf("Testing atomic32_exchange\n");
    init_event(&_exp_xchg, sizeof(struct ma_xchg_event));
    uint32_t val   = _exp_xchg.old.u32;
    _exp_xchg.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_exchange(&val, _exp_xchg.val.u32, _exp_xchg.mo);
    ensure(val == _exp_xchg.val.u32);
    ensure(ret == _exp_xchg.old.u32);
}
static DICE_CTOR void
register_atomic32_xchg(void)
{
    tests[test_count++] = test_atomic32_exchange;
}
uint64_t __tsan_atomic64_exchange(volatile uint64_t *a, uint64_t v, int mo);

static void
test_atomic64_exchange(void)
{
    log_printf("Testing atomic64_exchange\n");
    init_event(&_exp_xchg, sizeof(struct ma_xchg_event));
    uint64_t val   = _exp_xchg.old.u64;
    _exp_xchg.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_exchange(&val, _exp_xchg.val.u64, _exp_xchg.mo);
    ensure(val == _exp_xchg.val.u64);
    ensure(ret == _exp_xchg.old.u64);
}
static DICE_CTOR void
register_atomic64_xchg(void)
{
    tests[test_count++] = test_atomic64_exchange;
}

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_XCHG, {
    struct ma_xchg_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u8 == _exp_xchg.val.u8);
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u16 == _exp_xchg.val.u16);
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u32 == _exp_xchg.val.u32);
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u64 == _exp_xchg.val.u64);
        ensure(ev->old.u64 == 0);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_XCHG, {
    struct ma_xchg_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u8 == _exp_xchg.val.u8);
        ensure(ev->old.u8 == _exp_xchg.old.u8);
    } else if (strcmp(ev->func, "atomic16_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u16 == _exp_xchg.val.u16);
        ensure(ev->old.u16 == _exp_xchg.old.u16);
    } else if (strcmp(ev->func, "atomic32_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u32 == _exp_xchg.val.u32);
        ensure(ev->old.u32 == _exp_xchg.old.u32);
    } else if (strcmp(ev->func, "atomic64_exchange") == 0) {
        ensure(ev->addr == _exp_xchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_xchg.mo);
        ensure(ev->val.u64 == _exp_xchg.val.u64);
        ensure(ev->old.u64 == _exp_xchg.old.u64);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * fetch_RMW
 * -------------------------------------------------------------------------- */
struct ma_rmw_event _exp_rmw;


uint8_t __tsan_atomic8_fetch_add(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_add(void)
{
    log_printf("Testing atomic8_fetch_add\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_fetch_add(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp = (_exp_rmw.old.u8 + _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_add(void)
{
    tests[test_count++] = test_atomic8_fetch_add;
}
uint16_t __tsan_atomic16_fetch_add(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_add(void)
{
    log_printf("Testing atomic16_fetch_add\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_add(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = (_exp_rmw.old.u16 + _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_add(void)
{
    tests[test_count++] = test_atomic16_fetch_add;
}
uint32_t __tsan_atomic32_fetch_add(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_add(void)
{
    log_printf("Testing atomic32_fetch_add\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_add(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = (_exp_rmw.old.u32 + _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_add(void)
{
    tests[test_count++] = test_atomic32_fetch_add;
}
uint64_t __tsan_atomic64_fetch_add(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_add(void)
{
    log_printf("Testing atomic64_fetch_add\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_add(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = (_exp_rmw.old.u64 + _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_add(void)
{
    tests[test_count++] = test_atomic64_fetch_add;
}
uint8_t __tsan_atomic8_fetch_sub(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_sub(void)
{
    log_printf("Testing atomic8_fetch_sub\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_fetch_sub(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp = (_exp_rmw.old.u8 - _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_sub(void)
{
    tests[test_count++] = test_atomic8_fetch_sub;
}
uint16_t __tsan_atomic16_fetch_sub(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_sub(void)
{
    log_printf("Testing atomic16_fetch_sub\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_sub(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = (_exp_rmw.old.u16 - _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_sub(void)
{
    tests[test_count++] = test_atomic16_fetch_sub;
}
uint32_t __tsan_atomic32_fetch_sub(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_sub(void)
{
    log_printf("Testing atomic32_fetch_sub\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_sub(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = (_exp_rmw.old.u32 - _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_sub(void)
{
    tests[test_count++] = test_atomic32_fetch_sub;
}
uint64_t __tsan_atomic64_fetch_sub(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_sub(void)
{
    log_printf("Testing atomic64_fetch_sub\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_sub(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = (_exp_rmw.old.u64 - _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_sub(void)
{
    tests[test_count++] = test_atomic64_fetch_sub;
}
uint8_t __tsan_atomic8_fetch_and(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_and(void)
{
    log_printf("Testing atomic8_fetch_and\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_fetch_and(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp = (_exp_rmw.old.u8 & _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_and(void)
{
    tests[test_count++] = test_atomic8_fetch_and;
}
uint16_t __tsan_atomic16_fetch_and(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_and(void)
{
    log_printf("Testing atomic16_fetch_and\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_and(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = (_exp_rmw.old.u16 & _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_and(void)
{
    tests[test_count++] = test_atomic16_fetch_and;
}
uint32_t __tsan_atomic32_fetch_and(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_and(void)
{
    log_printf("Testing atomic32_fetch_and\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_and(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = (_exp_rmw.old.u32 & _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_and(void)
{
    tests[test_count++] = test_atomic32_fetch_and;
}
uint64_t __tsan_atomic64_fetch_and(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_and(void)
{
    log_printf("Testing atomic64_fetch_and\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_and(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = (_exp_rmw.old.u64 & _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_and(void)
{
    tests[test_count++] = test_atomic64_fetch_and;
}
uint8_t __tsan_atomic8_fetch_or(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_or(void)
{
    log_printf("Testing atomic8_fetch_or\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret   = __tsan_atomic8_fetch_or(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp   = (_exp_rmw.old.u8 | _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_or(void)
{
    tests[test_count++] = test_atomic8_fetch_or;
}
uint16_t __tsan_atomic16_fetch_or(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_or(void)
{
    log_printf("Testing atomic16_fetch_or\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_or(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = (_exp_rmw.old.u16 | _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_or(void)
{
    tests[test_count++] = test_atomic16_fetch_or;
}
uint32_t __tsan_atomic32_fetch_or(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_or(void)
{
    log_printf("Testing atomic32_fetch_or\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_or(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = (_exp_rmw.old.u32 | _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_or(void)
{
    tests[test_count++] = test_atomic32_fetch_or;
}
uint64_t __tsan_atomic64_fetch_or(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_or(void)
{
    log_printf("Testing atomic64_fetch_or\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_or(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = (_exp_rmw.old.u64 | _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_or(void)
{
    tests[test_count++] = test_atomic64_fetch_or;
}
uint8_t __tsan_atomic8_fetch_xor(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_xor(void)
{
    log_printf("Testing atomic8_fetch_xor\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_fetch_xor(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp = (_exp_rmw.old.u8 ^ _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_xor(void)
{
    tests[test_count++] = test_atomic8_fetch_xor;
}
uint16_t __tsan_atomic16_fetch_xor(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_xor(void)
{
    log_printf("Testing atomic16_fetch_xor\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_xor(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = (_exp_rmw.old.u16 ^ _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_xor(void)
{
    tests[test_count++] = test_atomic16_fetch_xor;
}
uint32_t __tsan_atomic32_fetch_xor(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_xor(void)
{
    log_printf("Testing atomic32_fetch_xor\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_xor(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = (_exp_rmw.old.u32 ^ _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_xor(void)
{
    tests[test_count++] = test_atomic32_fetch_xor;
}
uint64_t __tsan_atomic64_fetch_xor(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_xor(void)
{
    log_printf("Testing atomic64_fetch_xor\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_xor(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = (_exp_rmw.old.u64 ^ _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_xor(void)
{
    tests[test_count++] = test_atomic64_fetch_xor;
}
uint8_t __tsan_atomic8_fetch_nand(volatile uint8_t *a, uint8_t v, int mo);
static void
test_atomic8_fetch_nand(void)
{
    log_printf("Testing atomic8_fetch_nand\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint8_t val   = _exp_rmw.old.u8;
    _exp_rmw.addr = (void *)&val;
    uint8_t ret = __tsan_atomic8_fetch_nand(&val, _exp_rmw.val.u8, _exp_rmw.mo);
    uint8_t exp = ~(_exp_rmw.old.u8 & _exp_rmw.val.u8);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u8);
}
static DICE_CTOR void
register_atomic8_fetch_nand(void)
{
    tests[test_count++] = test_atomic8_fetch_nand;
}
uint16_t __tsan_atomic16_fetch_nand(volatile uint16_t *a, uint16_t v, int mo);
static void
test_atomic16_fetch_nand(void)
{
    log_printf("Testing atomic16_fetch_nand\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint16_t val  = _exp_rmw.old.u16;
    _exp_rmw.addr = (void *)&val;
    uint16_t ret =
        __tsan_atomic16_fetch_nand(&val, _exp_rmw.val.u16, _exp_rmw.mo);
    uint16_t exp = ~(_exp_rmw.old.u16 & _exp_rmw.val.u16);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u16);
}
static DICE_CTOR void
register_atomic16_fetch_nand(void)
{
    tests[test_count++] = test_atomic16_fetch_nand;
}
uint32_t __tsan_atomic32_fetch_nand(volatile uint32_t *a, uint32_t v, int mo);
static void
test_atomic32_fetch_nand(void)
{
    log_printf("Testing atomic32_fetch_nand\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint32_t val  = _exp_rmw.old.u32;
    _exp_rmw.addr = (void *)&val;
    uint32_t ret =
        __tsan_atomic32_fetch_nand(&val, _exp_rmw.val.u32, _exp_rmw.mo);
    uint32_t exp = ~(_exp_rmw.old.u32 & _exp_rmw.val.u32);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u32);
}
static DICE_CTOR void
register_atomic32_fetch_nand(void)
{
    tests[test_count++] = test_atomic32_fetch_nand;
}
uint64_t __tsan_atomic64_fetch_nand(volatile uint64_t *a, uint64_t v, int mo);
static void
test_atomic64_fetch_nand(void)
{
    log_printf("Testing atomic64_fetch_nand\n");
    init_event(&_exp_rmw, sizeof(struct ma_rmw_event));
    uint64_t val  = _exp_rmw.old.u64;
    _exp_rmw.addr = (void *)&val;
    uint64_t ret =
        __tsan_atomic64_fetch_nand(&val, _exp_rmw.val.u64, _exp_rmw.mo);
    uint64_t exp = ~(_exp_rmw.old.u64 & _exp_rmw.val.u64);
    ensure(val == exp && "after return");
    ensure(ret == _exp_rmw.old.u64);
}
static DICE_CTOR void
register_atomic64_fetch_nand(void)
{
    tests[test_count++] = test_atomic64_fetch_nand;
}

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_RMW, {
    struct ma_rmw_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else if (strcmp(ev->func, "atomic8_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else if (strcmp(ev->func, "atomic8_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else if (strcmp(ev->func, "atomic8_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else if (strcmp(ev->func, "atomic8_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else if (strcmp(ev->func, "atomic8_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
    } else if (strcmp(ev->func, "atomic16_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
    } else if (strcmp(ev->func, "atomic32_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
    } else if (strcmp(ev->func, "atomic64_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_RMW, {
    struct ma_rmw_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_add") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic8_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_sub") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic8_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_and") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic8_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_or") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic8_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_xor") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic8_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u8 == _exp_rmw.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_rmw.old.u8 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic16_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u16 == _exp_rmw.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_rmw.old.u16 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic32_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u32 == _exp_rmw.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_rmw.old.u32 && "INTERCEPT_AFTER");
    } else if (strcmp(ev->func, "atomic64_fetch_nand") == 0) {
        ensure(ev->addr == _exp_rmw.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_rmw.mo);
        ensure(ev->val.u64 == _exp_rmw.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_rmw.old.u64 && "INTERCEPT_AFTER");
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * compare_exchange_{strong,weak}
 * -------------------------------------------------------------------------- */
struct ma_cmpxchg_event _exp_cmpxchg;

int __tsan_atomic8_compare_exchange_strong(volatile uint8_t *a, uint8_t *c,
                                           uint8_t v, int mo);
static void
test_atomic8_compare_exchange_strong(void)
{
    int ret;

    log_printf("Testing atomic8_compare_exchange_strong sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint8_t var         = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 1;
    _exp_cmpxchg.cmp.u8 = _exp_cmpxchg.old.u8;
    uint8_t cmp         = _exp_cmpxchg.cmp.u8;

    ret = __tsan_atomic8_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u8, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u8);
    ensure(var == _exp_cmpxchg.val.u8);

    log_printf("Testing atomic8_compare_exchange_strong failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                 = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 0;
    _exp_cmpxchg.cmp.u8 = ~_exp_cmpxchg.old.u8;
    cmp                 = _exp_cmpxchg.cmp.u8;

    ret = __tsan_atomic8_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u8, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u8);
    ensure(var == _exp_cmpxchg.old.u8);
}

static DICE_CTOR void
register_atomic8_compare_exchange_strong(void)
{
    tests[test_count++] = test_atomic8_compare_exchange_strong;
}
int __tsan_atomic16_compare_exchange_strong(volatile uint16_t *a, uint16_t *c,
                                            uint16_t v, int mo);
static void
test_atomic16_compare_exchange_strong(void)
{
    int ret;

    log_printf("Testing atomic16_compare_exchange_strong sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint16_t var         = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u16 = _exp_cmpxchg.old.u16;
    uint16_t cmp         = _exp_cmpxchg.cmp.u16;

    ret = __tsan_atomic16_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u16);
    ensure(var == _exp_cmpxchg.val.u16);

    log_printf("Testing atomic16_compare_exchange_strong failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u16 = ~_exp_cmpxchg.old.u16;
    cmp                  = _exp_cmpxchg.cmp.u16;

    ret = __tsan_atomic16_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u16);
    ensure(var == _exp_cmpxchg.old.u16);
}

static DICE_CTOR void
register_atomic16_compare_exchange_strong(void)
{
    tests[test_count++] = test_atomic16_compare_exchange_strong;
}
int __tsan_atomic32_compare_exchange_strong(volatile uint32_t *a, uint32_t *c,
                                            uint32_t v, int mo);
static void
test_atomic32_compare_exchange_strong(void)
{
    int ret;

    log_printf("Testing atomic32_compare_exchange_strong sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint32_t var         = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u32 = _exp_cmpxchg.old.u32;
    uint32_t cmp         = _exp_cmpxchg.cmp.u32;

    ret = __tsan_atomic32_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u32);
    ensure(var == _exp_cmpxchg.val.u32);

    log_printf("Testing atomic32_compare_exchange_strong failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u32 = ~_exp_cmpxchg.old.u32;
    cmp                  = _exp_cmpxchg.cmp.u32;

    ret = __tsan_atomic32_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u32);
    ensure(var == _exp_cmpxchg.old.u32);
}

static DICE_CTOR void
register_atomic32_compare_exchange_strong(void)
{
    tests[test_count++] = test_atomic32_compare_exchange_strong;
}
int __tsan_atomic64_compare_exchange_strong(volatile uint64_t *a, uint64_t *c,
                                            uint64_t v, int mo);
static void
test_atomic64_compare_exchange_strong(void)
{
    int ret;

    log_printf("Testing atomic64_compare_exchange_strong sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint64_t var         = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u64 = _exp_cmpxchg.old.u64;
    uint64_t cmp         = _exp_cmpxchg.cmp.u64;

    ret = __tsan_atomic64_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u64);
    ensure(var == _exp_cmpxchg.val.u64);

    log_printf("Testing atomic64_compare_exchange_strong failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u64 = ~_exp_cmpxchg.old.u64;
    cmp                  = _exp_cmpxchg.cmp.u64;

    ret = __tsan_atomic64_compare_exchange_strong(
        &var, &cmp, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u64);
    ensure(var == _exp_cmpxchg.old.u64);
}

static DICE_CTOR void
register_atomic64_compare_exchange_strong(void)
{
    tests[test_count++] = test_atomic64_compare_exchange_strong;
}
int __tsan_atomic8_compare_exchange_weak(volatile uint8_t *a, uint8_t *c,
                                         uint8_t v, int mo);
static void
test_atomic8_compare_exchange_weak(void)
{
    int ret;

    log_printf("Testing atomic8_compare_exchange_weak sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint8_t var         = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 1;
    _exp_cmpxchg.cmp.u8 = _exp_cmpxchg.old.u8;
    uint8_t cmp         = _exp_cmpxchg.cmp.u8;

    ret = __tsan_atomic8_compare_exchange_weak(&var, &cmp, _exp_cmpxchg.val.u8,
                                               _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u8);
    ensure(var == _exp_cmpxchg.val.u8);

    log_printf("Testing atomic8_compare_exchange_weak failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                 = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 0;
    _exp_cmpxchg.cmp.u8 = ~_exp_cmpxchg.old.u8;
    cmp                 = _exp_cmpxchg.cmp.u8;

    ret = __tsan_atomic8_compare_exchange_weak(&var, &cmp, _exp_cmpxchg.val.u8,
                                               _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u8);
    ensure(var == _exp_cmpxchg.old.u8);
}

static DICE_CTOR void
register_atomic8_compare_exchange_weak(void)
{
    tests[test_count++] = test_atomic8_compare_exchange_weak;
}
int __tsan_atomic16_compare_exchange_weak(volatile uint16_t *a, uint16_t *c,
                                          uint16_t v, int mo);
static void
test_atomic16_compare_exchange_weak(void)
{
    int ret;

    log_printf("Testing atomic16_compare_exchange_weak sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint16_t var         = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u16 = _exp_cmpxchg.old.u16;
    uint16_t cmp         = _exp_cmpxchg.cmp.u16;

    ret = __tsan_atomic16_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u16);
    ensure(var == _exp_cmpxchg.val.u16);

    log_printf("Testing atomic16_compare_exchange_weak failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u16 = ~_exp_cmpxchg.old.u16;
    cmp                  = _exp_cmpxchg.cmp.u16;

    ret = __tsan_atomic16_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u16);
    ensure(var == _exp_cmpxchg.old.u16);
}

static DICE_CTOR void
register_atomic16_compare_exchange_weak(void)
{
    tests[test_count++] = test_atomic16_compare_exchange_weak;
}
int __tsan_atomic32_compare_exchange_weak(volatile uint32_t *a, uint32_t *c,
                                          uint32_t v, int mo);
static void
test_atomic32_compare_exchange_weak(void)
{
    int ret;

    log_printf("Testing atomic32_compare_exchange_weak sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint32_t var         = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u32 = _exp_cmpxchg.old.u32;
    uint32_t cmp         = _exp_cmpxchg.cmp.u32;

    ret = __tsan_atomic32_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u32);
    ensure(var == _exp_cmpxchg.val.u32);

    log_printf("Testing atomic32_compare_exchange_weak failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u32 = ~_exp_cmpxchg.old.u32;
    cmp                  = _exp_cmpxchg.cmp.u32;

    ret = __tsan_atomic32_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u32);
    ensure(var == _exp_cmpxchg.old.u32);
}

static DICE_CTOR void
register_atomic32_compare_exchange_weak(void)
{
    tests[test_count++] = test_atomic32_compare_exchange_weak;
}
int __tsan_atomic64_compare_exchange_weak(volatile uint64_t *a, uint64_t *c,
                                          uint64_t v, int mo);
static void
test_atomic64_compare_exchange_weak(void)
{
    int ret;

    log_printf("Testing atomic64_compare_exchange_weak sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint64_t var         = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u64 = _exp_cmpxchg.old.u64;
    uint64_t cmp         = _exp_cmpxchg.cmp.u64;

    ret = __tsan_atomic64_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == 1);
    ensure(cmp == _exp_cmpxchg.old.u64);
    ensure(var == _exp_cmpxchg.val.u64);

    log_printf("Testing atomic64_compare_exchange_weak failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u64 = ~_exp_cmpxchg.old.u64;
    cmp                  = _exp_cmpxchg.cmp.u64;

    ret = __tsan_atomic64_compare_exchange_weak(
        &var, &cmp, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == 0);
    ensure(cmp == _exp_cmpxchg.old.u64);
    ensure(var == _exp_cmpxchg.old.u64);
}

static DICE_CTOR void
register_atomic64_compare_exchange_weak(void)
{
    tests[test_count++] = test_atomic64_compare_exchange_weak;
}

uint8_t __tsan_atomic8_compare_exchange_val(volatile uint8_t *a, uint8_t c,
                                            uint8_t v, int mo);
static void
test_atomic8_compare_exchange_val(void)
{
    uint8_t ret;

    log_printf("Testing atomic8_compare_exchange_val sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint8_t var         = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 1;
    _exp_cmpxchg.cmp.u8 = _exp_cmpxchg.old.u8;

    ret = __tsan_atomic8_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u8, _exp_cmpxchg.val.u8, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u8);
    ensure(ret == _exp_cmpxchg.cmp.u8);
    ensure(_exp_cmpxchg.ok);
    ensure(var == _exp_cmpxchg.val.u8);

    log_printf("Testing atomic8_compare_exchange_val failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                 = _exp_cmpxchg.old.u8;
    _exp_cmpxchg.addr   = (void *)&var;
    _exp_cmpxchg.ok     = 0;
    _exp_cmpxchg.cmp.u8 = ~_exp_cmpxchg.old.u8;

    ret = __tsan_atomic8_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u8, _exp_cmpxchg.val.u8, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u8);
    ensure(var == _exp_cmpxchg.old.u8);
    ensure(ret != _exp_cmpxchg.cmp.u8);
    ensure(!_exp_cmpxchg.ok);
}

static DICE_CTOR void
register_atomic8_compare_exchange_val(void)
{
    tests[test_count++] = test_atomic8_compare_exchange_val;
}
uint16_t __tsan_atomic16_compare_exchange_val(volatile uint16_t *a, uint16_t c,
                                              uint16_t v, int mo);
static void
test_atomic16_compare_exchange_val(void)
{
    uint16_t ret;

    log_printf("Testing atomic16_compare_exchange_val sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint16_t var         = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u16 = _exp_cmpxchg.old.u16;

    ret = __tsan_atomic16_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u16, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u16);
    ensure(ret == _exp_cmpxchg.cmp.u16);
    ensure(_exp_cmpxchg.ok);
    ensure(var == _exp_cmpxchg.val.u16);

    log_printf("Testing atomic16_compare_exchange_val failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u16;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u16 = ~_exp_cmpxchg.old.u16;

    ret = __tsan_atomic16_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u16, _exp_cmpxchg.val.u16, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u16);
    ensure(var == _exp_cmpxchg.old.u16);
    ensure(ret != _exp_cmpxchg.cmp.u16);
    ensure(!_exp_cmpxchg.ok);
}

static DICE_CTOR void
register_atomic16_compare_exchange_val(void)
{
    tests[test_count++] = test_atomic16_compare_exchange_val;
}
uint32_t __tsan_atomic32_compare_exchange_val(volatile uint32_t *a, uint32_t c,
                                              uint32_t v, int mo);
static void
test_atomic32_compare_exchange_val(void)
{
    uint32_t ret;

    log_printf("Testing atomic32_compare_exchange_val sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint32_t var         = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u32 = _exp_cmpxchg.old.u32;

    ret = __tsan_atomic32_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u32, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u32);
    ensure(ret == _exp_cmpxchg.cmp.u32);
    ensure(_exp_cmpxchg.ok);
    ensure(var == _exp_cmpxchg.val.u32);

    log_printf("Testing atomic32_compare_exchange_val failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u32;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u32 = ~_exp_cmpxchg.old.u32;

    ret = __tsan_atomic32_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u32, _exp_cmpxchg.val.u32, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u32);
    ensure(var == _exp_cmpxchg.old.u32);
    ensure(ret != _exp_cmpxchg.cmp.u32);
    ensure(!_exp_cmpxchg.ok);
}

static DICE_CTOR void
register_atomic32_compare_exchange_val(void)
{
    tests[test_count++] = test_atomic32_compare_exchange_val;
}
uint64_t __tsan_atomic64_compare_exchange_val(volatile uint64_t *a, uint64_t c,
                                              uint64_t v, int mo);
static void
test_atomic64_compare_exchange_val(void)
{
    uint64_t ret;

    log_printf("Testing atomic64_compare_exchange_val sucess\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    uint64_t var         = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 1;
    _exp_cmpxchg.cmp.u64 = _exp_cmpxchg.old.u64;

    ret = __tsan_atomic64_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u64, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u64);
    ensure(ret == _exp_cmpxchg.cmp.u64);
    ensure(_exp_cmpxchg.ok);
    ensure(var == _exp_cmpxchg.val.u64);

    log_printf("Testing atomic64_compare_exchange_val failure\n");
    init_event(&_exp_cmpxchg, sizeof(struct ma_cmpxchg_event));
    var                  = _exp_cmpxchg.old.u64;
    _exp_cmpxchg.addr    = (void *)&var;
    _exp_cmpxchg.ok      = 0;
    _exp_cmpxchg.cmp.u64 = ~_exp_cmpxchg.old.u64;

    ret = __tsan_atomic64_compare_exchange_val(
        &var, _exp_cmpxchg.cmp.u64, _exp_cmpxchg.val.u64, _exp_cmpxchg.mo);

    ensure(ret == _exp_cmpxchg.old.u64);
    ensure(var == _exp_cmpxchg.old.u64);
    ensure(ret != _exp_cmpxchg.cmp.u64);
    ensure(!_exp_cmpxchg.ok);
}

static DICE_CTOR void
register_atomic64_compare_exchange_val(void)
{
    tests[test_count++] = test_atomic64_compare_exchange_val;
}
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_CMPXCHG, {
    struct ma_cmpxchg_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic8_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic8_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_BEFORE");
        ensure(ev->old.u8 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_BEFORE");
        ensure(ev->old.u16 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_BEFORE");
        ensure(ev->old.u32 == 0);
        ensure(ev->ok == 0);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_BEFORE");
        ensure(ev->old.u64 == 0);
        ensure(ev->ok == 0);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_CMPXCHG, {
    struct ma_cmpxchg_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic8_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_cmpxchg.old.u8 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_cmpxchg.old.u16 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_cmpxchg.old.u32 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_strong") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_cmpxchg.old.u64 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic8_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_cmpxchg.old.u8 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_cmpxchg.old.u16 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_cmpxchg.old.u32 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_weak") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_cmpxchg.old.u64 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic8_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (8 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u8 == _exp_cmpxchg.val.u8 && "INTERCEPT_AFTER");
        ensure(ev->old.u8 == _exp_cmpxchg.old.u8 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic16_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (16 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u16 == _exp_cmpxchg.val.u16 && "INTERCEPT_AFTER");
        ensure(ev->old.u16 == _exp_cmpxchg.old.u16 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic32_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (32 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u32 == _exp_cmpxchg.val.u32 && "INTERCEPT_AFTER");
        ensure(ev->old.u32 == _exp_cmpxchg.old.u32 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else if (strcmp(ev->func, "atomic64_compare_exchange_val") == 0) {
        ensure(ev->addr == _exp_cmpxchg.addr);
        ensure(ev->size == (64 >> 3));
        ensure(ev->mo == _exp_cmpxchg.mo);
        ensure(ev->val.u64 == _exp_cmpxchg.val.u64 && "INTERCEPT_AFTER");
        ensure(ev->old.u64 == _exp_cmpxchg.old.u64 && "INTERCEPT_AFTER");
        ensure(ev->ok == _exp_cmpxchg.ok);
    } else
        ensure(false && "unexpected");
});

/* -----------------------------------------------------------------------------
 * atomic fences
 * -------------------------------------------------------------------------- */
struct ma_fence_event _exp_fence;

void __tsan_atomic_thread_fence(int mo);
void
test_atomic_thread_fence(void)
{
    log_printf("Testing atomic_thread_fence\n");
    init_event(&_exp_fence, sizeof(struct ma_fence_event));
    __tsan_atomic_thread_fence(_exp_fence.mo);
}
static DICE_CTOR void
register_atomic_thread_fence(void)
{
    tests[test_count++] = test_atomic_thread_fence;
}
void __tsan_atomic_signal_fence(int mo);
void
test_atomic_signal_fence(void)
{
    log_printf("Testing atomic_signal_fence\n");
    init_event(&_exp_fence, sizeof(struct ma_fence_event));
    __tsan_atomic_signal_fence(_exp_fence.mo);
}
static DICE_CTOR void
register_atomic_signal_fence(void)
{
    tests[test_count++] = test_atomic_signal_fence;
}

PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MA_FENCE, {
    struct ma_fence_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic_thread_fence") == 0) {
        ensure(ev->mo == _exp_fence.mo);
    } else if (strcmp(ev->func, "atomic_signal_fence") == 0) {
        ensure(ev->mo == _exp_fence.mo);
    } else
        ensure(false && "unexpected");
});

PS_SUBSCRIBE(INTERCEPT_AFTER, EVENT_MA_FENCE, {
    struct ma_fence_event *ev = EVENT_PAYLOAD(ev);
    if (strcmp(ev->func, "atomic_thread_fence") == 0) {
        ensure(ev->mo == _exp_fence.mo);
    } else if (strcmp(ev->func, "atomic_signal_fence") == 0) {
        ensure(ev->mo == _exp_fence.mo);
    } else
        ensure(false && "unexpected");
});


int
main(void)
{
    for (size_t i = test_count; i > 0; i--)
        tests[i - 1]();
    return 0;
}
