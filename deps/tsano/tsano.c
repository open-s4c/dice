/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * @file tsano.c
 * @brief Implements an empty TSAN interface.
 *
 * This file implements an empty TSAN interface. Atomic operations behave as
 * expected compiler builtins and seq_cst barriers.
 ******************************************************************************/
#include <stdint.h>
#include <string.h>

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
/* plain reads and writes */
void
__tsan_read1(void *a)
{
    (void)a;
}
void
__tsan_read2(void *a)
{
    (void)a;
}
void
__tsan_read4(void *a)
{
    (void)a;
}
void
__tsan_read8(void *a)
{
    (void)a;
}
void
__tsan_read16(void *a)
{
    (void)a;
}
void
__tsan_write1(void *a)
{
    (void)a;
}
void
__tsan_write2(void *a)
{
    (void)a;
}
void
__tsan_write4(void *a)
{
    (void)a;
}
void
__tsan_write8(void *a)
{
    (void)a;
}
void
__tsan_write16(void *a)
{
    (void)a;
}
void
__tsan_unaligned_read1(void *a)
{
    (void)a;
}
void
__tsan_unaligned_read2(void *a)
{
    (void)a;
}
void
__tsan_unaligned_read4(void *a)
{
    (void)a;
}
void
__tsan_unaligned_read8(void *a)
{
    (void)a;
}
void
__tsan_unaligned_read16(void *a)
{
    (void)a;
}
void
__tsan_unaligned_write1(void *a)
{
    (void)a;
}
void
__tsan_unaligned_write2(void *a)
{
    (void)a;
}
void
__tsan_unaligned_write4(void *a)
{
    (void)a;
}
void
__tsan_unaligned_write8(void *a)
{
    (void)a;
}
void
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
__tsan_atomic8_compare_exchange_weak(volatile uint8_t *a, uint8_t *c, uint8_t v,
                                     int mo)
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
__tsan_atomic8_compare_exchange_val(volatile uint8_t *a, uint8_t c, uint8_t v,
                                    int mo)
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

void
AnnotateHappensBefore(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
AnnotateHappensAfter(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
AnnotateCondVarSignal(char *f, int l, void *cv)
{
    (void)f;
    (void)l;
    (void)cv;
}

void
AnnotateCondVarSignalAll(char *f, int l, void *cv)
{
    (void)f;
    (void)l;
    (void)cv;
}

void
AnnotateMutexIsNotPHB(char *f, int l, void *mu)
{
    (void)f;
    (void)l;
    (void)mu;
}

void
AnnotateCondVarWait(char *f, int l, void *cv, void *lock)
{
    (void)f;
    (void)l;
    (void)cv;
    (void)lock;
}

void
AnnotateRWLockCreate(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockCreateStatic(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockDestroy(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockAcquired(char *f, int l, void *m, void *is_w)
{
    (void)f;
    (void)l;
    (void)m;
    (void)is_w;
}

void
AnnotateRWLockReleased(char *f, int l, void *m, void *is_w)
{
    (void)f;
    (void)l;
    (void)m;
    (void)is_w;
}

void
AnnotateTraceMemory(char *f, int l, void *mem)
{
    (void)f;
    (void)l;
    (void)mem;
}

void
AnnotateFlushState(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateNewMemory(char *f, int l, void *mem, void *size)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)size;
}

void
AnnotateNoOp(char *f, int l, void *mem)
{
    (void)f;
    (void)l;
    (void)mem;
}

void
AnnotateFlushExpectedRaces(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateEnableRaceDetection(char *f, int l, int enable)
{
    (void)f;
    (void)l;
    (void)enable;
}

void
AnnotateMutexIsUsedAsCondVar(char *f, int l, void *mu)
{
    (void)f;
    (void)l;
    (void)mu;
}

void
AnnotatePCQGet(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQPut(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQDestroy(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQCreate(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotateExpectRace(char *f, int l, void *mem, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)desc;
}

void
AnnotateBenignRaceSized(char *f, int l, void *mem, void *size, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)size;
    (void)desc;
}

void
AnnotateBenignRace(char *f, int l, void *mem, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)desc;
}

void
AnnotateIgnoreReadsBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreReadsEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreWritesBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreWritesEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreSyncBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreSyncEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotatePublishMemoryRange(char *f, int l, void *addr, void *size)
{
    (void)f;
    (void)l;
    (void)addr;
    (void)size;
}

void
AnnotateUnpublishMemoryRange(char *f, int l, void *addr, void *size)
{
    (void)f;
    (void)l;
    (void)addr;
    (void)size;
}

void
AnnotateThreadName(char *f, int l, char *name)
{
    (void)f;
    (void)l;
    (void)name;
}

void
WTFAnnotateHappensBefore(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
WTFAnnotateHappensAfter(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
WTFAnnotateBenignRaceSized(char *f, int l, void *mem, void *sz, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
    (void)desc;
}

void
AnnotateMemoryIsInitialized(char *f, int l, void *mem, void *sz)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
}

void
AnnotateMemoryIsUninitialized(char *f, int l, void *mem, void *sz)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
}

void *
__tsan_get_current_fiber(void)
{
    return (void *)0;
}

void *
__tsan_create_fiber(unsigned flags)
{
    (void)flags;
    return (void *)0;
}

void
__tsan_destroy_fiber(void *fiber)
{
    (void)fiber;
}

void
__tsan_switch_to_fiber(void *fiber, unsigned flags)
{
    (void)fiber;
    (void)flags;
}

void
__tsan_set_fiber_name(void *fiber, const char *name)
{
    (void)fiber;
    (void)name;
}

void
__sanitizer_syscall_pre_impl_time(long tloc)
{
}

void
__sanitizer_syscall_post_impl_time(long res, long tloc)
{
}

void
__sanitizer_syscall_pre_impl_stime(long tptr)
{
}

void
__sanitizer_syscall_post_impl_stime(long res, long tptr)
{
}

void
__sanitizer_syscall_pre_impl_gettimeofday(long tv, long tz)
{
}

void
__sanitizer_syscall_post_impl_gettimeofday(long res, long tv, long tz)
{
}

void
__sanitizer_syscall_pre_impl_settimeofday(long tv, long tz)
{
}

void
__sanitizer_syscall_post_impl_settimeofday(long res, long tv, long tz)
{
}

void
__sanitizer_syscall_pre_impl_adjtimex(long txc_p)
{
}

void
__sanitizer_syscall_post_impl_adjtimex(long res, long txc_p)
{
}

void
__sanitizer_syscall_pre_impl_times(long tbuf)
{
}

void
__sanitizer_syscall_post_impl_times(long res, long tbuf)
{
}

void
__sanitizer_syscall_pre_impl_gettid()
{
}

void
__sanitizer_syscall_post_impl_gettid(long res)
{
}

void
__sanitizer_syscall_pre_impl_nanosleep(long rqtp, long rmtp)
{
}

void
__sanitizer_syscall_post_impl_nanosleep(long res, long rqtp, long rmtp)
{
}

void
__sanitizer_syscall_pre_impl_alarm(long seconds)
{
}

void
__sanitizer_syscall_post_impl_alarm(long res, long seconds)
{
}

void
__sanitizer_syscall_pre_impl_getpid()
{
}

void
__sanitizer_syscall_post_impl_getpid(long res)
{
}

void
__sanitizer_syscall_pre_impl_getppid()
{
}

void
__sanitizer_syscall_post_impl_getppid(long res)
{
}

void
__sanitizer_syscall_pre_impl_getuid()
{
}

void
__sanitizer_syscall_post_impl_getuid(long res)
{
}

void
__sanitizer_syscall_pre_impl_geteuid()
{
}

void
__sanitizer_syscall_post_impl_geteuid(long res)
{
}

void
__sanitizer_syscall_pre_impl_getgid()
{
}

void
__sanitizer_syscall_post_impl_getgid(long res)
{
}

void
__sanitizer_syscall_pre_impl_getegid()
{
}

void
__sanitizer_syscall_post_impl_getegid(long res)
{
}

void
__sanitizer_syscall_pre_impl_getresuid(long ruid, long euid, long suid)
{
}

void
__sanitizer_syscall_post_impl_getresuid(long res, long ruid, long euid,
                                        long suid)
{
}

void
__sanitizer_syscall_pre_impl_getresgid(long rgid, long egid, long sgid)
{
}

void
__sanitizer_syscall_post_impl_getresgid(long res, long rgid, long egid,
                                        long sgid)
{
}

void
__sanitizer_syscall_pre_impl_getpgid(long pid)
{
}

void
__sanitizer_syscall_post_impl_getpgid(long res, long pid)
{
}

void
__sanitizer_syscall_pre_impl_getpgrp()
{
}

void
__sanitizer_syscall_post_impl_getpgrp(long res)
{
}

void
__sanitizer_syscall_pre_impl_getsid(long pid)
{
}

void
__sanitizer_syscall_post_impl_getsid(long res, long pid)
{
}

void
__sanitizer_syscall_pre_impl_getgroups(long gidsetsize, long grouplist)
{
}

void
__sanitizer_syscall_post_impl_getgroups(long res, long gidsetsize,
                                        long grouplist)
{
}

void
__sanitizer_syscall_pre_impl_setregid(long rgid, long egid)
{
}

void
__sanitizer_syscall_post_impl_setregid(long res, long rgid, long egid)
{
}

void
__sanitizer_syscall_pre_impl_setgid(long gid)
{
}

void
__sanitizer_syscall_post_impl_setgid(long res, long gid)
{
}

void
__sanitizer_syscall_pre_impl_setreuid(long ruid, long euid)
{
}

void
__sanitizer_syscall_post_impl_setreuid(long res, long ruid, long euid)
{
}

void
__sanitizer_syscall_pre_impl_setuid(long uid)
{
}

void
__sanitizer_syscall_post_impl_setuid(long res, long uid)
{
}

void
__sanitizer_syscall_pre_impl_setresuid(long ruid, long euid, long suid)
{
}

void
__sanitizer_syscall_post_impl_setresuid(long res, long ruid, long euid,
                                        long suid)
{
}

void
__sanitizer_syscall_pre_impl_setresgid(long rgid, long egid, long sgid)
{
}

void
__sanitizer_syscall_post_impl_setresgid(long res, long rgid, long egid,
                                        long sgid)
{
}

void
__sanitizer_syscall_pre_impl_setfsuid(long uid)
{
}

void
__sanitizer_syscall_post_impl_setfsuid(long res, long uid)
{
}

void
__sanitizer_syscall_pre_impl_setfsgid(long gid)
{
}

void
__sanitizer_syscall_post_impl_setfsgid(long res, long gid)
{
}

void
__sanitizer_syscall_pre_impl_setpgid(long pid, long pgid)
{
}

void
__sanitizer_syscall_post_impl_setpgid(long res, long pid, long pgid)
{
}

void
__sanitizer_syscall_pre_impl_setsid()
{
}

void
__sanitizer_syscall_post_impl_setsid(long res)
{
}

void
__sanitizer_syscall_pre_impl_setgroups(long gidsetsize, long grouplist)
{
}

void
__sanitizer_syscall_post_impl_setgroups(long res, long gidsetsize,
                                        long grouplist)
{
}

void
__sanitizer_syscall_pre_impl_acct(long name)
{
}

void
__sanitizer_syscall_post_impl_acct(long res, long name)
{
}

void
__sanitizer_syscall_pre_impl_capget(long header, long dataptr)
{
}

void
__sanitizer_syscall_post_impl_capget(long res, long header, long dataptr)
{
}

void
__sanitizer_syscall_pre_impl_capset(long header, long data)
{
}

void
__sanitizer_syscall_post_impl_capset(long res, long header, long data)
{
}

void
__sanitizer_syscall_pre_impl_personality(long personality)
{
}

void
__sanitizer_syscall_post_impl_personality(long res, long personality)
{
}

void
__sanitizer_syscall_pre_impl_sigpending(long set)
{
}

void
__sanitizer_syscall_post_impl_sigpending(long res, long set)
{
}

void
__sanitizer_syscall_pre_impl_sigprocmask(long how, long set, long oset)
{
}

void
__sanitizer_syscall_post_impl_sigprocmask(long res, long how, long set,
                                          long oset)
{
}

void
__sanitizer_syscall_pre_impl_getitimer(long which, long value)
{
}

void
__sanitizer_syscall_post_impl_getitimer(long res, long which, long value)
{
}

void
__sanitizer_syscall_pre_impl_setitimer(long which, long value, long ovalue)
{
}

void
__sanitizer_syscall_post_impl_setitimer(long res, long which, long value,
                                        long ovalue)
{
}

void
__sanitizer_syscall_pre_impl_timer_create(long which_clock,
                                          long timer_event_spec,
                                          long created_timer_id)
{
}

void
__sanitizer_syscall_post_impl_timer_create(long res, long which_clock,
                                           long timer_event_spec,
                                           long created_timer_id)
{
}

void
__sanitizer_syscall_pre_impl_timer_gettime(long timer_id, long setting)
{
}

void
__sanitizer_syscall_post_impl_timer_gettime(long res, long timer_id,
                                            long setting)
{
}

void
__sanitizer_syscall_pre_impl_timer_getoverrun(long timer_id)
{
}

void
__sanitizer_syscall_post_impl_timer_getoverrun(long res, long timer_id)
{
}

void
__sanitizer_syscall_pre_impl_timer_settime(long timer_id, long flags,
                                           long new_setting, long old_setting)
{
}

void
__sanitizer_syscall_post_impl_timer_settime(long res, long timer_id, long flags,
                                            long new_setting, long old_setting)
{
}

void
__sanitizer_syscall_pre_impl_timer_delete(long timer_id)
{
}

void
__sanitizer_syscall_post_impl_timer_delete(long res, long timer_id)
{
}

void
__sanitizer_syscall_pre_impl_clock_settime(long which_clock, long tp)
{
}

void
__sanitizer_syscall_post_impl_clock_settime(long res, long which_clock, long tp)
{
}

void
__sanitizer_syscall_pre_impl_clock_gettime(long which_clock, long tp)
{
}

void
__sanitizer_syscall_post_impl_clock_gettime(long res, long which_clock, long tp)
{
}

void
__sanitizer_syscall_pre_impl_clock_adjtime(long which_clock, long tx)
{
}

void
__sanitizer_syscall_post_impl_clock_adjtime(long res, long which_clock, long tx)
{
}

void
__sanitizer_syscall_pre_impl_clock_getres(long which_clock, long tp)
{
}

void
__sanitizer_syscall_post_impl_clock_getres(long res, long which_clock, long tp)
{
}

void
__sanitizer_syscall_pre_impl_clock_nanosleep(long which_clock, long flags,
                                             long rqtp, long rmtp)
{
}

void
__sanitizer_syscall_post_impl_clock_nanosleep(long res, long which_clock,
                                              long flags, long rqtp, long rmtp)
{
}

void
__sanitizer_syscall_pre_impl_nice(long increment)
{
}

void
__sanitizer_syscall_post_impl_nice(long res, long increment)
{
}

void
__sanitizer_syscall_pre_impl_sched_setscheduler(long pid, long policy,
                                                long param)
{
}

void
__sanitizer_syscall_post_impl_sched_setscheduler(long res, long pid,
                                                 long policy, long param)
{
}

void
__sanitizer_syscall_pre_impl_sched_setparam(long pid, long param)
{
}

void
__sanitizer_syscall_post_impl_sched_setparam(long res, long pid, long param)
{
}

void
__sanitizer_syscall_pre_impl_sched_getscheduler(long pid)
{
}

void
__sanitizer_syscall_post_impl_sched_getscheduler(long res, long pid)
{
}

void
__sanitizer_syscall_pre_impl_sched_getparam(long pid, long param)
{
}

void
__sanitizer_syscall_post_impl_sched_getparam(long res, long pid, long param)
{
}

void
__sanitizer_syscall_pre_impl_sched_setaffinity(long pid, long len,
                                               long user_mask_ptr)
{
}

void
__sanitizer_syscall_post_impl_sched_setaffinity(long res, long pid, long len,
                                                long user_mask_ptr)
{
}

void
__sanitizer_syscall_pre_impl_sched_getaffinity(long pid, long len,
                                               long user_mask_ptr)
{
}

void
__sanitizer_syscall_post_impl_sched_getaffinity(long res, long pid, long len,
                                                long user_mask_ptr)
{
}

void
__sanitizer_syscall_pre_impl_sched_yield()
{
}

void
__sanitizer_syscall_post_impl_sched_yield(long res)
{
}

void
__sanitizer_syscall_pre_impl_sched_get_priority_max(long policy)
{
}

void
__sanitizer_syscall_post_impl_sched_get_priority_max(long res, long policy)
{
}

void
__sanitizer_syscall_pre_impl_sched_get_priority_min(long policy)
{
}

void
__sanitizer_syscall_post_impl_sched_get_priority_min(long res, long policy)
{
}

void
__sanitizer_syscall_pre_impl_sched_rr_get_interval(long pid, long interval)
{
}

void
__sanitizer_syscall_post_impl_sched_rr_get_interval(long res, long pid,
                                                    long interval)
{
}

void
__sanitizer_syscall_pre_impl_setpriority(long which, long who, long niceval)
{
}

void
__sanitizer_syscall_post_impl_setpriority(long res, long which, long who,
                                          long niceval)
{
}

void
__sanitizer_syscall_pre_impl_getpriority(long which, long who)
{
}

void
__sanitizer_syscall_post_impl_getpriority(long res, long which, long who)
{
}

void
__sanitizer_syscall_pre_impl_shutdown(long arg0, long arg1)
{
}

void
__sanitizer_syscall_post_impl_shutdown(long res, long arg0, long arg1)
{
}

void
__sanitizer_syscall_pre_impl_reboot(long magic1, long magic2, long cmd,
                                    long arg)
{
}

void
__sanitizer_syscall_post_impl_reboot(long res, long magic1, long magic2,
                                     long cmd, long arg)
{
}

void
__sanitizer_syscall_pre_impl_restart_syscall()
{
}

void
__sanitizer_syscall_post_impl_restart_syscall(long res)
{
}

void
__sanitizer_syscall_pre_impl_kexec_load(long entry, long nr_segments,
                                        long segments, long flags)
{
}

void
__sanitizer_syscall_post_impl_kexec_load(long res, long entry, long nr_segments,
                                         long segments, long flags)
{
}

void
__sanitizer_syscall_pre_impl_exit(long error_code)
{
}

void
__sanitizer_syscall_post_impl_exit(long res, long error_code)
{
}

void
__sanitizer_syscall_pre_impl_exit_group(long error_code)
{
}

void
__sanitizer_syscall_post_impl_exit_group(long res, long error_code)
{
}

void
__sanitizer_syscall_pre_impl_wait4(long pid, long stat_addr, long options,
                                   long ru)
{
}

void
__sanitizer_syscall_post_impl_wait4(long res, long pid, long stat_addr,
                                    long options, long ru)
{
}

void
__sanitizer_syscall_pre_impl_waitid(long which, long pid, long infop,
                                    long options, long ru)
{
}

void
__sanitizer_syscall_post_impl_waitid(long res, long which, long pid, long infop,
                                     long options, long ru)
{
}

void
__sanitizer_syscall_pre_impl_waitpid(long pid, long stat_addr, long options)
{
}

void
__sanitizer_syscall_post_impl_waitpid(long res, long pid, long stat_addr,
                                      long options)
{
}

void
__sanitizer_syscall_pre_impl_set_tid_address(long tidptr)
{
}

void
__sanitizer_syscall_post_impl_set_tid_address(long res, long tidptr)
{
}

void
__sanitizer_syscall_pre_impl_init_module(long umod, long len, long uargs)
{
}

void
__sanitizer_syscall_post_impl_init_module(long res, long umod, long len,
                                          long uargs)
{
}

void
__sanitizer_syscall_pre_impl_delete_module(long name_user, long flags)
{
}

void
__sanitizer_syscall_post_impl_delete_module(long res, long name_user,
                                            long flags)
{
}

void
__sanitizer_syscall_pre_impl_rt_sigprocmask(long how, long set, long oset,
                                            long sigsetsize)
{
}

void
__sanitizer_syscall_post_impl_rt_sigprocmask(long res, long how, long set,
                                             long oset, long sigsetsize)
{
}

void
__sanitizer_syscall_pre_impl_rt_sigpending(long set, long sigsetsize)
{
}

void
__sanitizer_syscall_post_impl_rt_sigpending(long res, long set, long sigsetsize)
{
}

void
__sanitizer_syscall_pre_impl_rt_sigtimedwait(long uthese, long uinfo, long uts,
                                             long sigsetsize)
{
}

void
__sanitizer_syscall_post_impl_rt_sigtimedwait(long res, long uthese, long uinfo,
                                              long uts, long sigsetsize)
{
}

void
__sanitizer_syscall_pre_impl_rt_tgsigqueueinfo(long tgid, long pid, long sig,
                                               long uinfo)
{
}

void
__sanitizer_syscall_post_impl_rt_tgsigqueueinfo(long res, long tgid, long pid,
                                                long sig, long uinfo)
{
}

void
__sanitizer_syscall_pre_impl_kill(long pid, long sig)
{
}

void
__sanitizer_syscall_post_impl_kill(long res, long pid, long sig)
{
}

void
__sanitizer_syscall_pre_impl_tgkill(long tgid, long pid, long sig)
{
}

void
__sanitizer_syscall_post_impl_tgkill(long res, long tgid, long pid, long sig)
{
}

void
__sanitizer_syscall_pre_impl_tkill(long pid, long sig)
{
}

void
__sanitizer_syscall_post_impl_tkill(long res, long pid, long sig)
{
}

void
__sanitizer_syscall_pre_impl_rt_sigqueueinfo(long pid, long sig, long uinfo)
{
}

void
__sanitizer_syscall_post_impl_rt_sigqueueinfo(long res, long pid, long sig,
                                              long uinfo)
{
}

void
__sanitizer_syscall_pre_impl_sgetmask()
{
}

void
__sanitizer_syscall_post_impl_sgetmask(long res)
{
}

void
__sanitizer_syscall_pre_impl_ssetmask(long newmask)
{
}

void
__sanitizer_syscall_post_impl_ssetmask(long res, long newmask)
{
}

void
__sanitizer_syscall_pre_impl_signal(long sig, long handler)
{
}

void
__sanitizer_syscall_post_impl_signal(long res, long sig, long handler)
{
}

void
__sanitizer_syscall_pre_impl_pause()
{
}

void
__sanitizer_syscall_post_impl_pause(long res)
{
}

void
__sanitizer_syscall_pre_impl_sync()
{
}

void
__sanitizer_syscall_post_impl_sync(long res)
{
}

void
__sanitizer_syscall_pre_impl_fsync(long fd)
{
}

void
__sanitizer_syscall_post_impl_fsync(long res, long fd)
{
}

void
__sanitizer_syscall_pre_impl_fdatasync(long fd)
{
}

void
__sanitizer_syscall_post_impl_fdatasync(long res, long fd)
{
}

void
__sanitizer_syscall_pre_impl_bdflush(long func, long data)
{
}

void
__sanitizer_syscall_post_impl_bdflush(long res, long func, long data)
{
}

void
__sanitizer_syscall_pre_impl_mount(long dev_name, long dir_name, long type,
                                   long flags, long data)
{
}

void
__sanitizer_syscall_post_impl_mount(long res, long dev_name, long dir_name,
                                    long type, long flags, long data)
{
}

void
__sanitizer_syscall_pre_impl_umount(long name, long flags)
{
}

void
__sanitizer_syscall_post_impl_umount(long res, long name, long flags)
{
}

void
__sanitizer_syscall_pre_impl_oldumount(long name)
{
}

void
__sanitizer_syscall_post_impl_oldumount(long res, long name)
{
}

void
__sanitizer_syscall_pre_impl_truncate(long path, long length)
{
}

void
__sanitizer_syscall_post_impl_truncate(long res, long path, long length)
{
}

void
__sanitizer_syscall_pre_impl_ftruncate(long fd, long length)
{
}

void
__sanitizer_syscall_post_impl_ftruncate(long res, long fd, long length)
{
}

void
__sanitizer_syscall_pre_impl_stat(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_stat(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_statfs(long path, long buf)
{
}

void
__sanitizer_syscall_post_impl_statfs(long res, long path, long buf)
{
}

void
__sanitizer_syscall_pre_impl_statfs64(long path, long sz, long buf)
{
}

void
__sanitizer_syscall_post_impl_statfs64(long res, long path, long sz, long buf)
{
}

void
__sanitizer_syscall_pre_impl_fstatfs(long fd, long buf)
{
}

void
__sanitizer_syscall_post_impl_fstatfs(long res, long fd, long buf)
{
}

void
__sanitizer_syscall_pre_impl_fstatfs64(long fd, long sz, long buf)
{
}

void
__sanitizer_syscall_post_impl_fstatfs64(long res, long fd, long sz, long buf)
{
}

void
__sanitizer_syscall_pre_impl_lstat(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_lstat(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_fstat(long fd, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_fstat(long res, long fd, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_newstat(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_newstat(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_newlstat(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_newlstat(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_newfstat(long fd, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_newfstat(long res, long fd, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_ustat(long dev, long ubuf)
{
}

void
__sanitizer_syscall_post_impl_ustat(long res, long dev, long ubuf)
{
}

void
__sanitizer_syscall_pre_impl_stat64(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_stat64(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_fstat64(long fd, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_fstat64(long res, long fd, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_lstat64(long filename, long statbuf)
{
}

void
__sanitizer_syscall_post_impl_lstat64(long res, long filename, long statbuf)
{
}

void
__sanitizer_syscall_pre_impl_setxattr(long path, long name, long value,
                                      long size, long flags)
{
}

void
__sanitizer_syscall_post_impl_setxattr(long res, long path, long name,
                                       long value, long size, long flags)
{
}

void
__sanitizer_syscall_pre_impl_lsetxattr(long path, long name, long value,
                                       long size, long flags)
{
}

void
__sanitizer_syscall_post_impl_lsetxattr(long res, long path, long name,
                                        long value, long size, long flags)
{
}

void
__sanitizer_syscall_pre_impl_fsetxattr(long fd, long name, long value,
                                       long size, long flags)
{
}

void
__sanitizer_syscall_post_impl_fsetxattr(long res, long fd, long name,
                                        long value, long size, long flags)
{
}

void
__sanitizer_syscall_pre_impl_getxattr(long path, long name, long value,
                                      long size)
{
}

void
__sanitizer_syscall_post_impl_getxattr(long res, long path, long name,
                                       long value, long size)
{
}

void
__sanitizer_syscall_pre_impl_lgetxattr(long path, long name, long value,
                                       long size)
{
}

void
__sanitizer_syscall_post_impl_lgetxattr(long res, long path, long name,
                                        long value, long size)
{
}

void
__sanitizer_syscall_pre_impl_fgetxattr(long fd, long name, long value,
                                       long size)
{
}

void
__sanitizer_syscall_post_impl_fgetxattr(long res, long fd, long name,
                                        long value, long size)
{
}

void
__sanitizer_syscall_pre_impl_listxattr(long path, long list, long size)
{
}

void
__sanitizer_syscall_post_impl_listxattr(long res, long path, long list,
                                        long size)
{
}

void
__sanitizer_syscall_pre_impl_llistxattr(long path, long list, long size)
{
}

void
__sanitizer_syscall_post_impl_llistxattr(long res, long path, long list,
                                         long size)
{
}

void
__sanitizer_syscall_pre_impl_flistxattr(long fd, long list, long size)
{
}

void
__sanitizer_syscall_post_impl_flistxattr(long res, long fd, long list,
                                         long size)
{
}

void
__sanitizer_syscall_pre_impl_removexattr(long path, long name)
{
}

void
__sanitizer_syscall_post_impl_removexattr(long res, long path, long name)
{
}

void
__sanitizer_syscall_pre_impl_lremovexattr(long path, long name)
{
}

void
__sanitizer_syscall_post_impl_lremovexattr(long res, long path, long name)
{
}

void
__sanitizer_syscall_pre_impl_fremovexattr(long fd, long name)
{
}

void
__sanitizer_syscall_post_impl_fremovexattr(long res, long fd, long name)
{
}

void
__sanitizer_syscall_pre_impl_brk(long brk)
{
}

void
__sanitizer_syscall_post_impl_brk(long res, long brk)
{
}

void
__sanitizer_syscall_pre_impl_mprotect(long start, long len, long prot)
{
}

void
__sanitizer_syscall_post_impl_mprotect(long res, long start, long len,
                                       long prot)
{
}

void
__sanitizer_syscall_pre_impl_mremap(long addr, long old_len, long new_len,
                                    long flags, long new_addr)
{
}

void
__sanitizer_syscall_post_impl_mremap(long res, long addr, long old_len,
                                     long new_len, long flags, long new_addr)
{
}

void
__sanitizer_syscall_pre_impl_remap_file_pages(long start, long size, long prot,
                                              long pgoff, long flags)
{
}

void
__sanitizer_syscall_post_impl_remap_file_pages(long res, long start, long size,
                                               long prot, long pgoff,
                                               long flags)
{
}

void
__sanitizer_syscall_pre_impl_msync(long start, long len, long flags)
{
}

void
__sanitizer_syscall_post_impl_msync(long res, long start, long len, long flags)
{
}

void
__sanitizer_syscall_pre_impl_munmap(long addr, long len)
{
}

void
__sanitizer_syscall_post_impl_munmap(long res, long addr, long len)
{
}

void
__sanitizer_syscall_pre_impl_mlock(long start, long len)
{
}

void
__sanitizer_syscall_post_impl_mlock(long res, long start, long len)
{
}

void
__sanitizer_syscall_pre_impl_munlock(long start, long len)
{
}

void
__sanitizer_syscall_post_impl_munlock(long res, long start, long len)
{
}

void
__sanitizer_syscall_pre_impl_mlockall(long flags)
{
}

void
__sanitizer_syscall_post_impl_mlockall(long res, long flags)
{
}

void
__sanitizer_syscall_pre_impl_munlockall()
{
}

void
__sanitizer_syscall_post_impl_munlockall(long res)
{
}

void
__sanitizer_syscall_pre_impl_madvise(long start, long len, long behavior)
{
}

void
__sanitizer_syscall_post_impl_madvise(long res, long start, long len,
                                      long behavior)
{
}

void
__sanitizer_syscall_pre_impl_mincore(long start, long len, long vec)
{
}

void
__sanitizer_syscall_post_impl_mincore(long res, long start, long len, long vec)
{
}

void
__sanitizer_syscall_pre_impl_pivot_root(long new_root, long put_old)
{
}

void
__sanitizer_syscall_post_impl_pivot_root(long res, long new_root, long put_old)
{
}

void
__sanitizer_syscall_pre_impl_chroot(long filename)
{
}

void
__sanitizer_syscall_post_impl_chroot(long res, long filename)
{
}

void
__sanitizer_syscall_pre_impl_mknod(long filename, long mode, long dev)
{
}

void
__sanitizer_syscall_post_impl_mknod(long res, long filename, long mode,
                                    long dev)
{
}

void
__sanitizer_syscall_pre_impl_link(long oldname, long newname)
{
}

void
__sanitizer_syscall_post_impl_link(long res, long oldname, long newname)
{
}

void
__sanitizer_syscall_pre_impl_symlink(long old, long new_)
{
}

void
__sanitizer_syscall_post_impl_symlink(long res, long old, long new_)
{
}

void
__sanitizer_syscall_pre_impl_unlink(long pathname)
{
}

void
__sanitizer_syscall_post_impl_unlink(long res, long pathname)
{
}

void
__sanitizer_syscall_pre_impl_rename(long oldname, long newname)
{
}

void
__sanitizer_syscall_post_impl_rename(long res, long oldname, long newname)
{
}

void
__sanitizer_syscall_pre_impl_chmod(long filename, long mode)
{
}

void
__sanitizer_syscall_post_impl_chmod(long res, long filename, long mode)
{
}

void
__sanitizer_syscall_pre_impl_fchmod(long fd, long mode)
{
}

void
__sanitizer_syscall_post_impl_fchmod(long res, long fd, long mode)
{
}

void
__sanitizer_syscall_pre_impl_fcntl(long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_post_impl_fcntl(long res, long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_pre_impl_fcntl64(long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_post_impl_fcntl64(long res, long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_pre_impl_pipe(long fildes)
{
}

void
__sanitizer_syscall_post_impl_pipe(long res, long fildes)
{
}

void
__sanitizer_syscall_pre_impl_pipe2(long fildes, long flags)
{
}

void
__sanitizer_syscall_post_impl_pipe2(long res, long fildes, long flags)
{
}

void
__sanitizer_syscall_pre_impl_dup(long fildes)
{
}

void
__sanitizer_syscall_post_impl_dup(long res, long fildes)
{
}

void
__sanitizer_syscall_pre_impl_dup2(long oldfd, long newfd)
{
}

void
__sanitizer_syscall_post_impl_dup2(long res, long oldfd, long newfd)
{
}

void
__sanitizer_syscall_pre_impl_dup3(long oldfd, long newfd, long flags)
{
}

void
__sanitizer_syscall_post_impl_dup3(long res, long oldfd, long newfd, long flags)
{
}

void
__sanitizer_syscall_pre_impl_ioperm(long from, long num, long on)
{
}

void
__sanitizer_syscall_post_impl_ioperm(long res, long from, long num, long on)
{
}

void
__sanitizer_syscall_pre_impl_ioctl(long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_post_impl_ioctl(long res, long fd, long cmd, long arg)
{
}

void
__sanitizer_syscall_pre_impl_flock(long fd, long cmd)
{
}

void
__sanitizer_syscall_post_impl_flock(long res, long fd, long cmd)
{
}

void
__sanitizer_syscall_pre_impl_io_setup(long nr_reqs, long ctx)
{
}

void
__sanitizer_syscall_post_impl_io_setup(long res, long nr_reqs, long ctx)
{
}

void
__sanitizer_syscall_pre_impl_io_destroy(long ctx)
{
}

void
__sanitizer_syscall_post_impl_io_destroy(long res, long ctx)
{
}

void
__sanitizer_syscall_pre_impl_io_getevents(long ctx_id, long min_nr, long nr,
                                          long events, long timeout)
{
}

void
__sanitizer_syscall_post_impl_io_getevents(long res, long ctx_id, long min_nr,
                                           long nr, long events, long timeout)
{
}

void
__sanitizer_syscall_pre_impl_io_submit(long ctx_id, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_io_submit(long res, long ctx_id, long arg1,
                                        long arg2)
{
}

void
__sanitizer_syscall_pre_impl_io_cancel(long ctx_id, long iocb, long result)
{
}

void
__sanitizer_syscall_post_impl_io_cancel(long res, long ctx_id, long iocb,
                                        long result)
{
}

void
__sanitizer_syscall_pre_impl_sendfile(long out_fd, long in_fd, long offset,
                                      long count)
{
}

void
__sanitizer_syscall_post_impl_sendfile(long res, long out_fd, long in_fd,
                                       long offset, long count)
{
}

void
__sanitizer_syscall_pre_impl_sendfile64(long out_fd, long in_fd, long offset,
                                        long count)
{
}

void
__sanitizer_syscall_post_impl_sendfile64(long res, long out_fd, long in_fd,
                                         long offset, long count)
{
}

void
__sanitizer_syscall_pre_impl_readlink(long path, long buf, long bufsiz)
{
}

void
__sanitizer_syscall_post_impl_readlink(long res, long path, long buf,
                                       long bufsiz)
{
}

void
__sanitizer_syscall_pre_impl_creat(long pathname, long mode)
{
}

void
__sanitizer_syscall_post_impl_creat(long res, long pathname, long mode)
{
}

void
__sanitizer_syscall_pre_impl_open(long filename, long flags, long mode)
{
}

void
__sanitizer_syscall_post_impl_open(long res, long filename, long flags,
                                   long mode)
{
}

void
__sanitizer_syscall_pre_impl_close(long fd)
{
}

void
__sanitizer_syscall_post_impl_close(long res, long fd)
{
}

void
__sanitizer_syscall_pre_impl_access(long filename, long mode)
{
}

void
__sanitizer_syscall_post_impl_access(long res, long filename, long mode)
{
}

void
__sanitizer_syscall_pre_impl_vhangup()
{
}

void
__sanitizer_syscall_post_impl_vhangup(long res)
{
}

void
__sanitizer_syscall_pre_impl_chown(long filename, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_chown(long res, long filename, long user,
                                    long group)
{
}

void
__sanitizer_syscall_pre_impl_lchown(long filename, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_lchown(long res, long filename, long user,
                                     long group)
{
}

void
__sanitizer_syscall_pre_impl_fchown(long fd, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_fchown(long res, long fd, long user, long group)
{
}

void
__sanitizer_syscall_pre_impl_chown16(long filename, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_chown16(long res, long filename, long user,
                                      long group)
{
}

void
__sanitizer_syscall_pre_impl_lchown16(long filename, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_lchown16(long res, long filename, long user,
                                       long group)
{
}

void
__sanitizer_syscall_pre_impl_fchown16(long fd, long user, long group)
{
}

void
__sanitizer_syscall_post_impl_fchown16(long res, long fd, long user, long group)
{
}

void
__sanitizer_syscall_pre_impl_setregid16(long rgid, long egid)
{
}

void
__sanitizer_syscall_post_impl_setregid16(long res, long rgid, long egid)
{
}

void
__sanitizer_syscall_pre_impl_setgid16(long gid)
{
}

void
__sanitizer_syscall_post_impl_setgid16(long res, long gid)
{
}

void
__sanitizer_syscall_pre_impl_setreuid16(long ruid, long euid)
{
}

void
__sanitizer_syscall_post_impl_setreuid16(long res, long ruid, long euid)
{
}

void
__sanitizer_syscall_pre_impl_setuid16(long uid)
{
}

void
__sanitizer_syscall_post_impl_setuid16(long res, long uid)
{
}

void
__sanitizer_syscall_pre_impl_setresuid16(long ruid, long euid, long suid)
{
}

void
__sanitizer_syscall_post_impl_setresuid16(long res, long ruid, long euid,
                                          long suid)
{
}

void
__sanitizer_syscall_pre_impl_getresuid16(long ruid, long euid, long suid)
{
}

void
__sanitizer_syscall_post_impl_getresuid16(long res, long ruid, long euid,
                                          long suid)
{
}

void
__sanitizer_syscall_pre_impl_setresgid16(long rgid, long egid, long sgid)
{
}

void
__sanitizer_syscall_post_impl_setresgid16(long res, long rgid, long egid,
                                          long sgid)
{
}

void
__sanitizer_syscall_pre_impl_getresgid16(long rgid, long egid, long sgid)
{
}

void
__sanitizer_syscall_post_impl_getresgid16(long res, long rgid, long egid,
                                          long sgid)
{
}

void
__sanitizer_syscall_pre_impl_setfsuid16(long uid)
{
}

void
__sanitizer_syscall_post_impl_setfsuid16(long res, long uid)
{
}

void
__sanitizer_syscall_pre_impl_setfsgid16(long gid)
{
}

void
__sanitizer_syscall_post_impl_setfsgid16(long res, long gid)
{
}

void
__sanitizer_syscall_pre_impl_getgroups16(long gidsetsize, long grouplist)
{
}

void
__sanitizer_syscall_post_impl_getgroups16(long res, long gidsetsize,
                                          long grouplist)
{
}

void
__sanitizer_syscall_pre_impl_setgroups16(long gidsetsize, long grouplist)
{
}

void
__sanitizer_syscall_post_impl_setgroups16(long res, long gidsetsize,
                                          long grouplist)
{
}

void
__sanitizer_syscall_pre_impl_getuid16()
{
}

void
__sanitizer_syscall_post_impl_getuid16(long res)
{
}

void
__sanitizer_syscall_pre_impl_geteuid16()
{
}

void
__sanitizer_syscall_post_impl_geteuid16(long res)
{
}

void
__sanitizer_syscall_pre_impl_getgid16()
{
}

void
__sanitizer_syscall_post_impl_getgid16(long res)
{
}

void
__sanitizer_syscall_pre_impl_getegid16()
{
}

void
__sanitizer_syscall_post_impl_getegid16(long res)
{
}

void
__sanitizer_syscall_pre_impl_utime(long filename, long times)
{
}

void
__sanitizer_syscall_post_impl_utime(long res, long filename, long times)
{
}

void
__sanitizer_syscall_pre_impl_utimes(long filename, long utimes)
{
}

void
__sanitizer_syscall_post_impl_utimes(long res, long filename, long utimes)
{
}

void
__sanitizer_syscall_pre_impl_lseek(long fd, long offset, long origin)
{
}

void
__sanitizer_syscall_post_impl_lseek(long res, long fd, long offset, long origin)
{
}

void
__sanitizer_syscall_pre_impl_llseek(long fd, long offset_high, long offset_low,
                                    long result, long origin)
{
}

void
__sanitizer_syscall_post_impl_llseek(long res, long fd, long offset_high,
                                     long offset_low, long result, long origin)
{
}

void
__sanitizer_syscall_pre_impl_read(long fd, long buf, long count)
{
}

void
__sanitizer_syscall_post_impl_read(long res, long fd, long buf, long count)
{
}

void
__sanitizer_syscall_pre_impl_readv(long fd, long vec, long vlen)
{
}

void
__sanitizer_syscall_post_impl_readv(long res, long fd, long vec, long vlen)
{
}

void
__sanitizer_syscall_pre_impl_write(long fd, long buf, long count)
{
}

void
__sanitizer_syscall_post_impl_write(long res, long fd, long buf, long count)
{
}

void
__sanitizer_syscall_pre_impl_writev(long fd, long vec, long vlen)
{
}

void
__sanitizer_syscall_post_impl_writev(long res, long fd, long vec, long vlen)
{
}


#ifdef _LP64
void
__sanitizer_syscall_pre_impl_pread64(long fd, long buf, long count, long pos)
{
}

void
__sanitizer_syscall_post_impl_pread64(long res, long fd, long buf, long count,
                                      long pos)
{
}

void
__sanitizer_syscall_pre_impl_pwrite64(long fd, long buf, long count, long pos)
{
}

void
__sanitizer_syscall_post_impl_pwrite64(long res, long fd, long buf, long count,
                                       long pos)
{
}

#else
void
__sanitizer_syscall_pre_impl_pread64(long fd, long buf, long count, long pos0,
                                     long pos1)
{
}

void
__sanitizer_syscall_post_impl_pread64(long res, long fd, long buf, long count,
                                      long pos0, long pos1)
{
}

void
__sanitizer_syscall_pre_impl_pwrite64(long fd, long buf, long count, long pos0,
                                      long pos1)
{
}

void
__sanitizer_syscall_post_impl_pwrite64(long res, long fd, long buf, long count,
                                       long pos0, long pos1)
{
}

#endif

void
__sanitizer_syscall_pre_impl_preadv(long fd, long vec, long vlen, long pos_l,
                                    long pos_h)
{
}

void
__sanitizer_syscall_post_impl_preadv(long res, long fd, long vec, long vlen,
                                     long pos_l, long pos_h)
{
}

void
__sanitizer_syscall_pre_impl_pwritev(long fd, long vec, long vlen, long pos_l,
                                     long pos_h)
{
}

void
__sanitizer_syscall_post_impl_pwritev(long res, long fd, long vec, long vlen,
                                      long pos_l, long pos_h)
{
}

void
__sanitizer_syscall_pre_impl_getcwd(long buf, long size)
{
}

void
__sanitizer_syscall_post_impl_getcwd(long res, long buf, long size)
{
}

void
__sanitizer_syscall_pre_impl_mkdir(long pathname, long mode)
{
}

void
__sanitizer_syscall_post_impl_mkdir(long res, long pathname, long mode)
{
}

void
__sanitizer_syscall_pre_impl_chdir(long filename)
{
}

void
__sanitizer_syscall_post_impl_chdir(long res, long filename)
{
}

void
__sanitizer_syscall_pre_impl_fchdir(long fd)
{
}

void
__sanitizer_syscall_post_impl_fchdir(long res, long fd)
{
}

void
__sanitizer_syscall_pre_impl_rmdir(long pathname)
{
}

void
__sanitizer_syscall_post_impl_rmdir(long res, long pathname)
{
}

void
__sanitizer_syscall_pre_impl_lookup_dcookie(long cookie64, long buf, long len)
{
}

void
__sanitizer_syscall_post_impl_lookup_dcookie(long res, long cookie64, long buf,
                                             long len)
{
}

void
__sanitizer_syscall_pre_impl_quotactl(long cmd, long special, long id,
                                      long addr)
{
}

void
__sanitizer_syscall_post_impl_quotactl(long res, long cmd, long special,
                                       long id, long addr)
{
}

void
__sanitizer_syscall_pre_impl_getdents(long fd, long dirent, long count)
{
}

void
__sanitizer_syscall_post_impl_getdents(long res, long fd, long dirent,
                                       long count)
{
}

void
__sanitizer_syscall_pre_impl_getdents64(long fd, long dirent, long count)
{
}

void
__sanitizer_syscall_post_impl_getdents64(long res, long fd, long dirent,
                                         long count)
{
}

void
__sanitizer_syscall_pre_impl_setsockopt(long fd, long level, long optname,
                                        long optval, long optlen)
{
}

void
__sanitizer_syscall_post_impl_setsockopt(long res, long fd, long level,
                                         long optname, long optval, long optlen)
{
}

void
__sanitizer_syscall_pre_impl_getsockopt(long fd, long level, long optname,
                                        long optval, long optlen)
{
}

void
__sanitizer_syscall_post_impl_getsockopt(long res, long fd, long level,
                                         long optname, long optval, long optlen)
{
}

void
__sanitizer_syscall_pre_impl_bind(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_bind(long res, long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_pre_impl_connect(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_connect(long res, long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_pre_impl_accept(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_accept(long res, long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_pre_impl_accept4(long arg0, long arg1, long arg2, long arg3)
{
}

void
__sanitizer_syscall_post_impl_accept4(long res, long arg0, long arg1, long arg2,
                                      long arg3)
{
}

void
__sanitizer_syscall_pre_impl_getsockname(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_getsockname(long res, long arg0, long arg1,
                                          long arg2)
{
}

void
__sanitizer_syscall_pre_impl_getpeername(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_getpeername(long res, long arg0, long arg1,
                                          long arg2)
{
}

void
__sanitizer_syscall_pre_impl_send(long arg0, long arg1, long arg2, long arg3)
{
}

void
__sanitizer_syscall_post_impl_send(long res, long arg0, long arg1, long arg2,
                                   long arg3)
{
}

void
__sanitizer_syscall_pre_impl_sendto(long arg0, long arg1, long arg2, long arg3,
                                    long arg4, long arg5)
{
}

void
__sanitizer_syscall_post_impl_sendto(long res, long arg0, long arg1, long arg2,
                                     long arg3, long arg4, long arg5)
{
}

void
__sanitizer_syscall_pre_impl_sendmsg(long fd, long msg, long flags)
{
}

void
__sanitizer_syscall_post_impl_sendmsg(long res, long fd, long msg, long flags)
{
}

void
__sanitizer_syscall_pre_impl_sendmmsg(long fd, long msg, long vlen, long flags)
{
}

void
__sanitizer_syscall_post_impl_sendmmsg(long res, long fd, long msg, long vlen,
                                       long flags)
{
}

void
__sanitizer_syscall_pre_impl_recv(long arg0, long arg1, long arg2, long arg3)
{
}

void
__sanitizer_syscall_post_impl_recv(long res, long arg0, long arg1, long arg2,
                                   long arg3)
{
}

void
__sanitizer_syscall_pre_impl_recvfrom(long arg0, long arg1, long arg2,
                                      long arg3, long arg4, long arg5)
{
}

void
__sanitizer_syscall_post_impl_recvfrom(long res, long arg0, long arg1,
                                       long arg2, long arg3, long arg4,
                                       long arg5)
{
}

void
__sanitizer_syscall_pre_impl_recvmsg(long fd, long msg, long flags)
{
}

void
__sanitizer_syscall_post_impl_recvmsg(long res, long fd, long msg, long flags)
{
}

void
__sanitizer_syscall_pre_impl_recvmmsg(long fd, long msg, long vlen, long flags,
                                      long timeout)
{
}

void
__sanitizer_syscall_post_impl_recvmmsg(long res, long fd, long msg, long vlen,
                                       long flags, long timeout)
{
}

void
__sanitizer_syscall_pre_impl_socket(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_socket(long res, long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_pre_impl_socketpair(long arg0, long arg1, long arg2,
                                        long arg3)
{
}

void
__sanitizer_syscall_post_impl_socketpair(long res, long arg0, long arg1,
                                         long arg2, long arg3)
{
}

void
__sanitizer_syscall_pre_impl_socketcall(long call, long args)
{
}

void
__sanitizer_syscall_post_impl_socketcall(long res, long call, long args)
{
}

void
__sanitizer_syscall_pre_impl_listen(long arg0, long arg1)
{
}

void
__sanitizer_syscall_post_impl_listen(long res, long arg0, long arg1)
{
}

void
__sanitizer_syscall_pre_impl_poll(long ufds, long nfds, long timeout)
{
}

void
__sanitizer_syscall_post_impl_poll(long res, long ufds, long nfds, long timeout)
{
}

void
__sanitizer_syscall_pre_impl_select(long n, long inp, long outp, long exp,
                                    long tvp)
{
}

void
__sanitizer_syscall_post_impl_select(long res, long n, long inp, long outp,
                                     long exp, long tvp)
{
}

void
__sanitizer_syscall_pre_impl_old_select(long arg)
{
}

void
__sanitizer_syscall_post_impl_old_select(long res, long arg)
{
}

void
__sanitizer_syscall_pre_impl_epoll_create(long size)
{
}

void
__sanitizer_syscall_post_impl_epoll_create(long res, long size)
{
}

void
__sanitizer_syscall_pre_impl_epoll_create1(long flags)
{
}

void
__sanitizer_syscall_post_impl_epoll_create1(long res, long flags)
{
}

void
__sanitizer_syscall_pre_impl_epoll_ctl(long epfd, long op, long fd, long event)
{
}

void
__sanitizer_syscall_post_impl_epoll_ctl(long res, long epfd, long op, long fd,
                                        long event)
{
}

void
__sanitizer_syscall_pre_impl_epoll_wait(long epfd, long events, long maxevents,
                                        long timeout)
{
}

void
__sanitizer_syscall_post_impl_epoll_wait(long res, long epfd, long events,
                                         long maxevents, long timeout)
{
}

void
__sanitizer_syscall_pre_impl_epoll_pwait(long epfd, long events, long maxevents,
                                         long timeout, long sigmask,
                                         long sigsetsize)
{
}

void
__sanitizer_syscall_post_impl_epoll_pwait(long res, long epfd, long events,
                                          long maxevents, long timeout,
                                          long sigmask, long sigsetsize)
{
}

void
__sanitizer_syscall_pre_impl_epoll_pwait2(long epfd, long events,
                                          long maxevents, long timeout,
                                          long sigmask, long sigsetsize)
{
}

void
__sanitizer_syscall_post_impl_epoll_pwait2(long res, long epfd, long events,
                                           long maxevents, long timeout,
                                           long sigmask, long sigsetsize)
{
}

void
__sanitizer_syscall_pre_impl_gethostname(long name, long len)
{
}

void
__sanitizer_syscall_post_impl_gethostname(long res, long name, long len)
{
}

void
__sanitizer_syscall_pre_impl_sethostname(long name, long len)
{
}

void
__sanitizer_syscall_post_impl_sethostname(long res, long name, long len)
{
}

void
__sanitizer_syscall_pre_impl_setdomainname(long name, long len)
{
}

void
__sanitizer_syscall_post_impl_setdomainname(long res, long name, long len)
{
}

void
__sanitizer_syscall_pre_impl_newuname(long name)
{
}

void
__sanitizer_syscall_post_impl_newuname(long res, long name)
{
}

void
__sanitizer_syscall_pre_impl_uname(long arg0)
{
}

void
__sanitizer_syscall_post_impl_uname(long res, long arg0)
{
}

void
__sanitizer_syscall_pre_impl_olduname(long arg0)
{
}

void
__sanitizer_syscall_post_impl_olduname(long res, long arg0)
{
}

void
__sanitizer_syscall_pre_impl_getrlimit(long resource, long rlim)
{
}

void
__sanitizer_syscall_post_impl_getrlimit(long res, long resource, long rlim)
{
}

void
__sanitizer_syscall_pre_impl_old_getrlimit(long resource, long rlim)
{
}

void
__sanitizer_syscall_post_impl_old_getrlimit(long res, long resource, long rlim)
{
}

void
__sanitizer_syscall_pre_impl_setrlimit(long resource, long rlim)
{
}

void
__sanitizer_syscall_post_impl_setrlimit(long res, long resource, long rlim)
{
}

void
__sanitizer_syscall_pre_impl_prlimit64(long pid, long resource, long new_rlim,
                                       long old_rlim)
{
}

void
__sanitizer_syscall_post_impl_prlimit64(long res, long pid, long resource,
                                        long new_rlim, long old_rlim)
{
}

void
__sanitizer_syscall_pre_impl_getrusage(long who, long ru)
{
}

void
__sanitizer_syscall_post_impl_getrusage(long res, long who, long ru)
{
}

void
__sanitizer_syscall_pre_impl_umask(long mask)
{
}

void
__sanitizer_syscall_post_impl_umask(long res, long mask)
{
}

void
__sanitizer_syscall_pre_impl_msgget(long key, long msgflg)
{
}

void
__sanitizer_syscall_post_impl_msgget(long res, long key, long msgflg)
{
}

void
__sanitizer_syscall_pre_impl_msgsnd(long msqid, long msgp, long msgsz,
                                    long msgflg)
{
}

void
__sanitizer_syscall_post_impl_msgsnd(long res, long msqid, long msgp,
                                     long msgsz, long msgflg)
{
}

void
__sanitizer_syscall_pre_impl_msgrcv(long msqid, long msgp, long msgsz,
                                    long msgtyp, long msgflg)
{
}

void
__sanitizer_syscall_post_impl_msgrcv(long res, long msqid, long msgp,
                                     long msgsz, long msgtyp, long msgflg)
{
}

void
__sanitizer_syscall_pre_impl_msgctl(long msqid, long cmd, long buf)
{
}

void
__sanitizer_syscall_post_impl_msgctl(long res, long msqid, long cmd, long buf)
{
}

void
__sanitizer_syscall_pre_impl_semget(long key, long nsems, long semflg)
{
}

void
__sanitizer_syscall_post_impl_semget(long res, long key, long nsems,
                                     long semflg)
{
}

void
__sanitizer_syscall_pre_impl_semop(long semid, long sops, long nsops)
{
}

void
__sanitizer_syscall_post_impl_semop(long res, long semid, long sops, long nsops)
{
}

void
__sanitizer_syscall_pre_impl_semctl(long semid, long semnum, long cmd, long arg)
{
}

void
__sanitizer_syscall_post_impl_semctl(long res, long semid, long semnum,
                                     long cmd, long arg)
{
}

void
__sanitizer_syscall_pre_impl_semtimedop(long semid, long sops, long nsops,
                                        long timeout)
{
}

void
__sanitizer_syscall_post_impl_semtimedop(long res, long semid, long sops,
                                         long nsops, long timeout)
{
}

void
__sanitizer_syscall_pre_impl_shmat(long shmid, long shmaddr, long shmflg)
{
}

void
__sanitizer_syscall_post_impl_shmat(long res, long shmid, long shmaddr,
                                    long shmflg)
{
}

void
__sanitizer_syscall_pre_impl_shmget(long key, long size, long flag)
{
}

void
__sanitizer_syscall_post_impl_shmget(long res, long key, long size, long flag)
{
}

void
__sanitizer_syscall_pre_impl_shmdt(long shmaddr)
{
}

void
__sanitizer_syscall_post_impl_shmdt(long res, long shmaddr)
{
}

void
__sanitizer_syscall_pre_impl_shmctl(long shmid, long cmd, long buf)
{
}

void
__sanitizer_syscall_post_impl_shmctl(long res, long shmid, long cmd, long buf)
{
}

void
__sanitizer_syscall_pre_impl_ipc(long call, long first, long second, long third,
                                 long ptr, long fifth)
{
}

void
__sanitizer_syscall_post_impl_ipc(long res, long call, long first, long second,
                                  long third, long ptr, long fifth)
{
}

void
__sanitizer_syscall_pre_impl_mq_open(long name, long oflag, long mode,
                                     long attr)
{
}

void
__sanitizer_syscall_post_impl_mq_open(long res, long name, long oflag,
                                      long mode, long attr)
{
}

void
__sanitizer_syscall_pre_impl_mq_unlink(long name)
{
}

void
__sanitizer_syscall_post_impl_mq_unlink(long res, long name)
{
}

void
__sanitizer_syscall_pre_impl_mq_timedsend(long mqdes, long msg_ptr,
                                          long msg_len, long msg_prio,
                                          long abs_timeout)
{
}

void
__sanitizer_syscall_post_impl_mq_timedsend(long res, long mqdes, long msg_ptr,
                                           long msg_len, long msg_prio,
                                           long abs_timeout)
{
}

void
__sanitizer_syscall_pre_impl_mq_timedreceive(long mqdes, long msg_ptr,
                                             long msg_len, long msg_prio,
                                             long abs_timeout)
{
}

void
__sanitizer_syscall_post_impl_mq_timedreceive(long res, long mqdes,
                                              long msg_ptr, long msg_len,
                                              long msg_prio, long abs_timeout)
{
}

void
__sanitizer_syscall_pre_impl_mq_notify(long mqdes, long notification)
{
}

void
__sanitizer_syscall_post_impl_mq_notify(long res, long mqdes, long notification)
{
}

void
__sanitizer_syscall_pre_impl_mq_getsetattr(long mqdes, long mqstat,
                                           long omqstat)
{
}

void
__sanitizer_syscall_post_impl_mq_getsetattr(long res, long mqdes, long mqstat,
                                            long omqstat)
{
}

void
__sanitizer_syscall_pre_impl_pciconfig_iobase(long which, long bus, long devfn)
{
}

void
__sanitizer_syscall_post_impl_pciconfig_iobase(long res, long which, long bus,
                                               long devfn)
{
}

void
__sanitizer_syscall_pre_impl_pciconfig_read(long bus, long dfn, long off,
                                            long len, long buf)
{
}

void
__sanitizer_syscall_post_impl_pciconfig_read(long res, long bus, long dfn,
                                             long off, long len, long buf)
{
}

void
__sanitizer_syscall_pre_impl_pciconfig_write(long bus, long dfn, long off,
                                             long len, long buf)
{
}

void
__sanitizer_syscall_post_impl_pciconfig_write(long res, long bus, long dfn,
                                              long off, long len, long buf)
{
}

void
__sanitizer_syscall_pre_impl_swapon(long specialfile, long swap_flags)
{
}

void
__sanitizer_syscall_post_impl_swapon(long res, long specialfile,
                                     long swap_flags)
{
}

void
__sanitizer_syscall_pre_impl_swapoff(long specialfile)
{
}

void
__sanitizer_syscall_post_impl_swapoff(long res, long specialfile)
{
}

void
__sanitizer_syscall_pre_impl_sysctl(long args)
{
}

void
__sanitizer_syscall_post_impl_sysctl(long res, long args)
{
}

void
__sanitizer_syscall_pre_impl_sysinfo(long info)
{
}

void
__sanitizer_syscall_post_impl_sysinfo(long res, long info)
{
}

void
__sanitizer_syscall_pre_impl_sysfs(long option, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_sysfs(long res, long option, long arg1, long arg2)
{
}

void
__sanitizer_syscall_pre_impl_syslog(long type, long buf, long len)
{
}

void
__sanitizer_syscall_post_impl_syslog(long res, long type, long buf, long len)
{
}

void
__sanitizer_syscall_pre_impl_uselib(long library)
{
}

void
__sanitizer_syscall_post_impl_uselib(long res, long library)
{
}

void
__sanitizer_syscall_pre_impl_ni_syscall()
{
}

void
__sanitizer_syscall_post_impl_ni_syscall(long res)
{
}

void
__sanitizer_syscall_pre_impl_ptrace(long request, long pid, long addr,
                                    long data)
{
}

void
__sanitizer_syscall_post_impl_ptrace(long res, long request, long pid,
                                     long addr, long data)
{
}

void
__sanitizer_syscall_pre_impl_add_key(long _type, long _description,
                                     long _payload, long plen, long destringid)
{
}

void
__sanitizer_syscall_post_impl_add_key(long res, long _type, long _description,
                                      long _payload, long plen, long destringid)
{
}

void
__sanitizer_syscall_pre_impl_request_key(long _type, long _description,
                                         long _callout_info, long destringid)
{
}

void
__sanitizer_syscall_post_impl_request_key(long res, long _type,
                                          long _description, long _callout_info,
                                          long destringid)
{
}

void
__sanitizer_syscall_pre_impl_keyctl(long cmd, long arg2, long arg3, long arg4,
                                    long arg5)
{
}

void
__sanitizer_syscall_post_impl_keyctl(long res, long cmd, long arg2, long arg3,
                                     long arg4, long arg5)
{
}

void
__sanitizer_syscall_pre_impl_ioprio_set(long which, long who, long ioprio)
{
}

void
__sanitizer_syscall_post_impl_ioprio_set(long res, long which, long who,
                                         long ioprio)
{
}

void
__sanitizer_syscall_pre_impl_ioprio_get(long which, long who)
{
}

void
__sanitizer_syscall_post_impl_ioprio_get(long res, long which, long who)
{
}

void
__sanitizer_syscall_pre_impl_set_mempolicy(long mode, long nmask, long maxnode)
{
}

void
__sanitizer_syscall_post_impl_set_mempolicy(long res, long mode, long nmask,
                                            long maxnode)
{
}

void
__sanitizer_syscall_pre_impl_migrate_pages(long pid, long maxnode, long from,
                                           long to)
{
}

void
__sanitizer_syscall_post_impl_migrate_pages(long res, long pid, long maxnode,
                                            long from, long to)
{
}

void
__sanitizer_syscall_pre_impl_move_pages(long pid, long nr_pages, long pages,
                                        long nodes, long status, long flags)
{
}

void
__sanitizer_syscall_post_impl_move_pages(long res, long pid, long nr_pages,
                                         long pages, long nodes, long status,
                                         long flags)
{
}

void
__sanitizer_syscall_pre_impl_mbind(long start, long len, long mode, long nmask,
                                   long maxnode, long flags)
{
}

void
__sanitizer_syscall_post_impl_mbind(long res, long start, long len, long mode,
                                    long nmask, long maxnode, long flags)
{
}

void
__sanitizer_syscall_pre_impl_get_mempolicy(long policy, long nmask,
                                           long maxnode, long addr, long flags)
{
}

void
__sanitizer_syscall_post_impl_get_mempolicy(long res, long policy, long nmask,
                                            long maxnode, long addr, long flags)
{
}

void
__sanitizer_syscall_pre_impl_inotify_init()
{
}

void
__sanitizer_syscall_post_impl_inotify_init(long res)
{
}

void
__sanitizer_syscall_pre_impl_inotify_init1(long flags)
{
}

void
__sanitizer_syscall_post_impl_inotify_init1(long res, long flags)
{
}

void
__sanitizer_syscall_pre_impl_inotify_add_watch(long fd, long path, long mask)
{
}

void
__sanitizer_syscall_post_impl_inotify_add_watch(long res, long fd, long path,
                                                long mask)
{
}

void
__sanitizer_syscall_pre_impl_inotify_rm_watch(long fd, long wd)
{
}

void
__sanitizer_syscall_post_impl_inotify_rm_watch(long res, long fd, long wd)
{
}

void
__sanitizer_syscall_pre_impl_spu_run(long fd, long unpc, long ustatus)
{
}

void
__sanitizer_syscall_post_impl_spu_run(long res, long fd, long unpc,
                                      long ustatus)
{
}

void
__sanitizer_syscall_pre_impl_spu_create(long name, long flags, long mode,
                                        long fd)
{
}

void
__sanitizer_syscall_post_impl_spu_create(long res, long name, long flags,
                                         long mode, long fd)
{
}

void
__sanitizer_syscall_pre_impl_mknodat(long dfd, long filename, long mode,
                                     long dev)
{
}

void
__sanitizer_syscall_post_impl_mknodat(long res, long dfd, long filename,
                                      long mode, long dev)
{
}

void
__sanitizer_syscall_pre_impl_mkdirat(long dfd, long pathname, long mode)
{
}

void
__sanitizer_syscall_post_impl_mkdirat(long res, long dfd, long pathname,
                                      long mode)
{
}

void
__sanitizer_syscall_pre_impl_unlinkat(long dfd, long pathname, long flag)
{
}

void
__sanitizer_syscall_post_impl_unlinkat(long res, long dfd, long pathname,
                                       long flag)
{
}

void
__sanitizer_syscall_pre_impl_symlinkat(long oldname, long newdfd, long newname)
{
}

void
__sanitizer_syscall_post_impl_symlinkat(long res, long oldname, long newdfd,
                                        long newname)
{
}

void
__sanitizer_syscall_pre_impl_linkat(long olddfd, long oldname, long newdfd,
                                    long newname, long flags)
{
}

void
__sanitizer_syscall_post_impl_linkat(long res, long olddfd, long oldname,
                                     long newdfd, long newname, long flags)
{
}

void
__sanitizer_syscall_pre_impl_renameat(long olddfd, long oldname, long newdfd,
                                      long newname)
{
}

void
__sanitizer_syscall_post_impl_renameat(long res, long olddfd, long oldname,
                                       long newdfd, long newname)
{
}

void
__sanitizer_syscall_pre_impl_futimesat(long dfd, long filename, long utimes)
{
}

void
__sanitizer_syscall_post_impl_futimesat(long res, long dfd, long filename,
                                        long utimes)
{
}

void
__sanitizer_syscall_pre_impl_faccessat(long dfd, long filename, long mode)
{
}

void
__sanitizer_syscall_post_impl_faccessat(long res, long dfd, long filename,
                                        long mode)
{
}

void
__sanitizer_syscall_pre_impl_fchmodat(long dfd, long filename, long mode)
{
}

void
__sanitizer_syscall_post_impl_fchmodat(long res, long dfd, long filename,
                                       long mode)
{
}

void
__sanitizer_syscall_pre_impl_fchownat(long dfd, long filename, long user,
                                      long group, long flag)
{
}

void
__sanitizer_syscall_post_impl_fchownat(long res, long dfd, long filename,
                                       long user, long group, long flag)
{
}

void
__sanitizer_syscall_pre_impl_openat(long dfd, long filename, long flags,
                                    long mode)
{
}

void
__sanitizer_syscall_post_impl_openat(long res, long dfd, long filename,
                                     long flags, long mode)
{
}

void
__sanitizer_syscall_pre_impl_newfstatat(long dfd, long filename, long statbuf,
                                        long flag)
{
}

void
__sanitizer_syscall_post_impl_newfstatat(long res, long dfd, long filename,
                                         long statbuf, long flag)
{
}

void
__sanitizer_syscall_pre_impl_fstatat64(long dfd, long filename, long statbuf,
                                       long flag)
{
}

void
__sanitizer_syscall_post_impl_fstatat64(long res, long dfd, long filename,
                                        long statbuf, long flag)
{
}

void
__sanitizer_syscall_pre_impl_readlinkat(long dfd, long path, long buf,
                                        long bufsiz)
{
}

void
__sanitizer_syscall_post_impl_readlinkat(long res, long dfd, long path,
                                         long buf, long bufsiz)
{
}

void
__sanitizer_syscall_pre_impl_utimensat(long dfd, long filename, long utimes,
                                       long flags)
{
}

void
__sanitizer_syscall_post_impl_utimensat(long res, long dfd, long filename,
                                        long utimes, long flags)
{
}

void
__sanitizer_syscall_pre_impl_unshare(long unshare_flags)
{
}

void
__sanitizer_syscall_post_impl_unshare(long res, long unshare_flags)
{
}

void
__sanitizer_syscall_pre_impl_splice(long fd_in, long off_in, long fd_out,
                                    long off_out, long len, long flags)
{
}

void
__sanitizer_syscall_post_impl_splice(long res, long fd_in, long off_in,
                                     long fd_out, long off_out, long len,
                                     long flags)
{
}

void
__sanitizer_syscall_pre_impl_vmsplice(long fd, long iov, long nr_segs,
                                      long flags)
{
}

void
__sanitizer_syscall_post_impl_vmsplice(long res, long fd, long iov,
                                       long nr_segs, long flags)
{
}

void
__sanitizer_syscall_pre_impl_tee(long fdin, long fdout, long len, long flags)
{
}

void
__sanitizer_syscall_post_impl_tee(long res, long fdin, long fdout, long len,
                                  long flags)
{
}

void
__sanitizer_syscall_pre_impl_get_robust_list(long pid, long head_ptr,
                                             long len_ptr)
{
}

void
__sanitizer_syscall_post_impl_get_robust_list(long res, long pid, long head_ptr,
                                              long len_ptr)
{
}

void
__sanitizer_syscall_pre_impl_set_robust_list(long head, long len)
{
}

void
__sanitizer_syscall_post_impl_set_robust_list(long res, long head, long len)
{
}

void
__sanitizer_syscall_pre_impl_getcpu(long cpu, long node, long cache)
{
}

void
__sanitizer_syscall_post_impl_getcpu(long res, long cpu, long node, long cache)
{
}

void
__sanitizer_syscall_pre_impl_signalfd(long ufd, long user_mask, long sizemask)
{
}

void
__sanitizer_syscall_post_impl_signalfd(long res, long ufd, long user_mask,
                                       long sizemask)
{
}

void
__sanitizer_syscall_pre_impl_signalfd4(long ufd, long user_mask, long sizemask,
                                       long flags)
{
}

void
__sanitizer_syscall_post_impl_signalfd4(long res, long ufd, long user_mask,
                                        long sizemask, long flags)
{
}

void
__sanitizer_syscall_pre_impl_timerfd_create(long clockid, long flags)
{
}

void
__sanitizer_syscall_post_impl_timerfd_create(long res, long clockid, long flags)
{
}

void
__sanitizer_syscall_pre_impl_timerfd_settime(long ufd, long flags, long utmr,
                                             long otmr)
{
}

void
__sanitizer_syscall_post_impl_timerfd_settime(long res, long ufd, long flags,
                                              long utmr, long otmr)
{
}

void
__sanitizer_syscall_pre_impl_timerfd_gettime(long ufd, long otmr)
{
}

void
__sanitizer_syscall_post_impl_timerfd_gettime(long res, long ufd, long otmr)
{
}

void
__sanitizer_syscall_pre_impl_eventfd(long count)
{
}

void
__sanitizer_syscall_post_impl_eventfd(long res, long count)
{
}

void
__sanitizer_syscall_pre_impl_eventfd2(long count, long flags)
{
}

void
__sanitizer_syscall_post_impl_eventfd2(long res, long count, long flags)
{
}

void
__sanitizer_syscall_pre_impl_old_readdir(long arg0, long arg1, long arg2)
{
}

void
__sanitizer_syscall_post_impl_old_readdir(long res, long arg0, long arg1,
                                          long arg2)
{
}

void
__sanitizer_syscall_pre_impl_pselect6(long arg0, long arg1, long arg2,
                                      long arg3, long arg4, long arg5)
{
}

void
__sanitizer_syscall_post_impl_pselect6(long res, long arg0, long arg1,
                                       long arg2, long arg3, long arg4,
                                       long arg5)
{
}

void
__sanitizer_syscall_pre_impl_ppoll(long arg0, long arg1, long arg2, long arg3,
                                   long arg4)
{
}

void
__sanitizer_syscall_post_impl_ppoll(long res, long arg0, long arg1, long arg2,
                                    long arg3, long arg4)
{
}

void
__sanitizer_syscall_pre_impl_fanotify_init(long flags, long event_f_flags)
{
}

void
__sanitizer_syscall_post_impl_fanotify_init(long res, long flags,
                                            long event_f_flags)
{
}

void
__sanitizer_syscall_pre_impl_fanotify_mark(long fanotify_fd, long flags,
                                           long mask, long fd, long pathname)
{
}

void
__sanitizer_syscall_post_impl_fanotify_mark(long res, long fanotify_fd,
                                            long flags, long mask, long fd,
                                            long pathname)
{
}

void
__sanitizer_syscall_pre_impl_syncfs(long fd)
{
}

void
__sanitizer_syscall_post_impl_syncfs(long res, long fd)
{
}

void
__sanitizer_syscall_pre_impl_perf_event_open(long attr_uptr, long pid, long cpu,
                                             long group_fd, long flags)
{
}

void
__sanitizer_syscall_post_impl_perf_event_open(long res, long attr_uptr,
                                              long pid, long cpu, long group_fd,
                                              long flags)
{
}

void
__sanitizer_syscall_pre_impl_mmap_pgoff(long addr, long len, long prot,
                                        long flags, long fd, long pgoff)
{
}

void
__sanitizer_syscall_post_impl_mmap_pgoff(long res, long addr, long len,
                                         long prot, long flags, long fd,
                                         long pgoff)
{
}

void
__sanitizer_syscall_pre_impl_old_mmap(long arg)
{
}

void
__sanitizer_syscall_post_impl_old_mmap(long res, long arg)
{
}

void
__sanitizer_syscall_pre_impl_name_to_handle_at(long dfd, long name, long handle,
                                               long mnt_id, long flag)
{
}

void
__sanitizer_syscall_post_impl_name_to_handle_at(long res, long dfd, long name,
                                                long handle, long mnt_id,
                                                long flag)
{
}

void
__sanitizer_syscall_pre_impl_open_by_handle_at(long mountdirfd, long handle,
                                               long flags)
{
}

void
__sanitizer_syscall_post_impl_open_by_handle_at(long res, long mountdirfd,
                                                long handle, long flags)
{
}

void
__sanitizer_syscall_pre_impl_setns(long fd, long nstype)
{
}

void
__sanitizer_syscall_post_impl_setns(long res, long fd, long nstype)
{
}

void
__sanitizer_syscall_pre_impl_process_vm_readv(long pid, long lvec, long liovcnt,
                                              long rvec, long riovcnt,
                                              long flags)
{
}

void
__sanitizer_syscall_post_impl_process_vm_readv(long res, long pid, long lvec,
                                               long liovcnt, long rvec,
                                               long riovcnt, long flags)
{
}

void
__sanitizer_syscall_pre_impl_process_vm_writev(long pid, long lvec,
                                               long liovcnt, long rvec,
                                               long riovcnt, long flags)
{
}

void
__sanitizer_syscall_post_impl_process_vm_writev(long res, long pid, long lvec,
                                                long liovcnt, long rvec,
                                                long riovcnt, long flags)
{
}

void
__sanitizer_syscall_pre_impl_fork()
{
}

void
__sanitizer_syscall_post_impl_fork(long res)
{
}

void
__sanitizer_syscall_pre_impl_vfork()
{
}

void
__sanitizer_syscall_post_impl_vfork(long res)
{
}

void
__sanitizer_syscall_pre_impl_sigaction(long signum, long act, long oldact)
{
}

void
__sanitizer_syscall_post_impl_sigaction(long res, long signum, long act,
                                        long oldact)
{
}

void
__sanitizer_syscall_pre_impl_rt_sigaction(long signum, long act, long oldact,
                                          long sz)
{
}

void
__sanitizer_syscall_post_impl_rt_sigaction(long res, long signum, long act,
                                           long oldact, long sz)
{
}

void
__sanitizer_syscall_pre_impl_sigaltstack(long ss, long oss)
{
}

void
__sanitizer_syscall_post_impl_sigaltstack(long res, long ss, long oss)
{
}

void
__sanitizer_syscall_pre_impl_futex(long uaddr, long futex_op, long val,
                                   long timeout, long uaddr2, long val3)
{
}

void
__sanitizer_syscall_post_impl_futex(long res, long uaddr, long futex_op,
                                    long val, long timeout, long uaddr2,
                                    long val3)
{
}