/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_ANNOTATE_RWLOCK_H
#define DICE_ANNOTATE_RWLOCK_H

#include <pthread.h>


void AnnotateRWLockCreate(const char *file, int line,
                          const volatile void *lock);
void AnnotateRWLockDestroy(const char *file, int line,
                           const volatile void *lock);
void AnnotateRWLockAcquired(const char *file, int line,
                            const volatile void *lock, long is_w);
void AnnotateRWLockReleased(const char *file, int line,
                            const volatile void *lock, long is_w);

#define EVENT_ANNOTATERWLOCKCREATE   43
#define EVENT_ANNOTATERWLOCKDESTROY  44
#define EVENT_ANNOTATERWLOCKACQUIRED 45
#define EVENT_ANNOTATERWLOCKRELEASED 46

struct AnnotateRWLockCreate_event {
    const char *file;
    int line;
    const volatile void *lock;
};

struct AnnotateRWLockDestroy_event {
    const char *file;
    int line;
    const volatile void *lock;
};

struct AnnotateRWLockAcquired_event {
    const char *file;
    int line;
    const volatile void *lock;
    long is_w;
};

struct AnnotateRWLockReleased_event {
    const char *file;
    int line;
    const volatile void *lock;
    long is_w;
};

#endif /* DICE_RW_LOCKS_H */
