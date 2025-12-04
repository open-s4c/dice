/*
 * Copyright (C) 2023-2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
/*******************************************************************************
 * @file tsano.c
 * @brief Implements an empty TSAN interface.
 *
 * This file implements an empty TSAN interface. Atomic operations behave as
 * expected compiler builtins and seq_cst barriers.
 ******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/* empty tsan initialization */
void
__tsan_init()
{
}

void
__tsan_read_range(void *addr, unsigned long size)
{
    (void)addr;
    (void)size;
}

void
__tsan_write_range(void *addr, unsigned long size)
{
    (void)addr;
    (void)size;
}

void
__tsan_read_range_pc(void *addr, unsigned long size, void *pc)
{
    (void)addr;
    (void)size;
    (void)pc;
}

void
__tsan_write_range_pc(void *addr, unsigned long size, void *pc)
{
    (void)addr;
    (void)size;
    (void)pc;
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
void
__tsan_func_entry(void *pc)
{
    (void)pc;
}
void
__tsan_func_exit(void)
{
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
void *
__tsan_memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}
/* C++ new and delete */
void *
_Znwm(size_t n)
{
    return malloc(n);
}
void *
_Znam(size_t n)
{
    return malloc(n);
}
void
_ZdlPv(void *ptr)
{
    return free(ptr);
}
void
_ZdaPv(void *ptr)
{
    return free(ptr);
}

/* plain reads and writes */
__attribute__((weak)) void
__tsan_read1(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_read2(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_read4(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_read8(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_read16(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_write1(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_write2(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_write4(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_write8(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_write16(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_read1(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_read2(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_read4(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_read8(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_read16(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_write1(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_write2(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_write4(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_write8(void *a)
{
    (void)a;
}
__attribute__((weak)) void
__tsan_unaligned_write16(void *a)
{
    (void)a;
}

/* plain reads and writes 2 values */
void
__tsan_read1_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_read2_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_read4_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_read8_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_read16_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_write1_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_write2_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_write4_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_write8_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}
void
__tsan_write16_pc(void *a, void *b)
{
    (void)a;
    (void)b;
}

/* atomic loads */
uint8_t
__tsan_atomic8_load(const volatile uint8_t *a, int mo)
{
    (void)mo;
    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_load(const volatile uint16_t *a, int mo)
{
    (void)mo;
    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_load(const volatile uint32_t *a, int mo)
{
    (void)mo;
    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_load(const volatile uint64_t *a, int mo)
{
    (void)mo;
    return __atomic_load_n(a, __ATOMIC_SEQ_CST);
}

/* atomic stores */
void
__tsan_atomic8_store(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
}
void
__tsan_atomic16_store(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
}
void
__tsan_atomic32_store(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
}
void
__tsan_atomic64_store(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    __atomic_store_n(a, v, __ATOMIC_SEQ_CST);
}


/* xchg */
uint8_t
__tsan_atomic8_exchange(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_exchange(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_exchange(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_exchange(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_exchange_n(a, v, __ATOMIC_SEQ_CST);
}

/* fetch_RMW */
uint8_t
__tsan_atomic8_fetch_add(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_add(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_add(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_add(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST);
}
uint8_t
__tsan_atomic8_fetch_sub(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_sub(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_sub(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_sub(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST);
}
uint8_t
__tsan_atomic8_fetch_and(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_and(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_and(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_and(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST);
}
uint8_t
__tsan_atomic8_fetch_or(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_or(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_or(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_or(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST);
}
uint8_t
__tsan_atomic8_fetch_xor(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_xor(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_xor(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_xor(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST);
}
uint8_t
__tsan_atomic8_fetch_nand(volatile uint8_t *a, uint8_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
}
uint16_t
__tsan_atomic16_fetch_nand(volatile uint16_t *a, uint16_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
}
uint32_t
__tsan_atomic32_fetch_nand(volatile uint32_t *a, uint32_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
}
uint64_t
__tsan_atomic64_fetch_nand(volatile uint64_t *a, uint64_t v, int mo)
{
    (void)mo;
    return __atomic_fetch_nand(a, v, __ATOMIC_SEQ_CST);
}


/* compare_exchange_{strong,weak} */
int
__tsan_atomic8_compare_exchange_strong(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic16_compare_exchange_strong(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic32_compare_exchange_strong(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic64_compare_exchange_strong(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 0, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic8_compare_exchange_weak(volatile uint8_t *a, uint8_t *c,
                                       uint8_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic16_compare_exchange_weak(volatile uint16_t *a, uint16_t *c,
                                       uint16_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic32_compare_exchange_weak(volatile uint32_t *a, uint32_t *c,
                                       uint32_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}
int
__tsan_atomic64_compare_exchange_weak(volatile uint64_t *a, uint64_t *c,
                                       uint64_t v, int mo)
{
    (void)mo;
    return __atomic_compare_exchange_n(a, c, v, 1, __ATOMIC_SEQ_CST,
                                       __ATOMIC_SEQ_CST);
}

/* compare_exchange_val */
uint8_t
__tsan_atomic8_compare_exchange_val(volatile uint8_t *a, uint8_t c,
                                       uint8_t v, int mo)
{
    (void)mo;
    (void)__atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                      __ATOMIC_SEQ_CST);
    return c;
}
uint16_t
__tsan_atomic16_compare_exchange_val(volatile uint16_t *a, uint16_t c,
                                       uint16_t v, int mo)
{
    (void)mo;
    (void)__atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                      __ATOMIC_SEQ_CST);
    return c;
}
uint32_t
__tsan_atomic32_compare_exchange_val(volatile uint32_t *a, uint32_t c,
                                       uint32_t v, int mo)
{
    (void)mo;
    (void)__atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                      __ATOMIC_SEQ_CST);
    return c;
}
uint64_t
__tsan_atomic64_compare_exchange_val(volatile uint64_t *a, uint64_t c,
                                       uint64_t v, int mo)
{
    (void)mo;
    (void)__atomic_compare_exchange_n(a, &c, v, 0, __ATOMIC_SEQ_CST,
                                      __ATOMIC_SEQ_CST);
    return c;
}

/* atomic fences */
void
__tsan_atomic_thread_fence(int mo)
{
    (void)mo;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}

void
__tsan_atomic_signal_fence(int mo)
{
    (void)mo;
    __atomic_signal_fence(__ATOMIC_SEQ_CST);
}
