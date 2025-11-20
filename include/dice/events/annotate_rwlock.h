/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_ANNOTATE_RWLOCK_H
#define DICE_ANNOTATE_RWLOCK_H

#include <pthread.h>

#include <dice/events/thread.h>


void AnnotateRWLockCreate(const char *file, int line,
                          const volatile void *lock);
void AnnotateRWLockDestroy(const char *file, int line,
                           const volatile void *lock);
void AnnotateRWLockAcquired(const char *file, int line,
                            const volatile void *lock, long is_w);
void AnnotateRWLockReleased(const char *file, int line,
                            const volatile void *lock, long is_w);

#define EVENT_ANNOTATE_RWLOCK_CREATE  43
#define EVENT_ANNOTATE_RWLOCK_DESTROY 44
#define EVENT_ANNOTATE_RWLOCK_ACQ     45
#define EVENT_ANNOTATE_RWLOCK_REL     46

#define EVENT_ANNOTATERWLOCKCREATE   EVENT_ANNOTATE_RWLOCK_CREATE
#define EVENT_ANNOTATERWLOCKDESTROY  EVENT_ANNOTATE_RWLOCK_DESTROY
#define EVENT_ANNOTATERWLOCKACQUIRED EVENT_ANNOTATE_RWLOCK_ACQ
#define EVENT_ANNOTATERWLOCKRELEASED EVENT_ANNOTATE_RWLOCK_REL

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
