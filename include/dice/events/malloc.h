/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_MALLOC_H
#define DICE_MALLOC_H

#include <stddef.h>

#include <dice/compiler.h>

#ifndef DICE_ROLLED
    #define EVENT_MALLOC             50
    #define EVENT_CALLOC             51
    #define EVENT_REALLOC            52
    #define EVENT_FREE               53
    #define EVENT_POSIX_MEMALIGN     54
    #define EVENT_ALIGNED_ALLOC      55
    #define EVENT_MALLOC_USABLE_SIZE 56
#endif

struct malloc_event {
    const void *pc;
    size_t size;
    void *ret;
    void *(*func)(size_t);
};

struct calloc_event {
    const void *pc;
    size_t number;
    size_t size;
    void *ret;
    void *(*func)(size_t, size_t);
};

struct realloc_event {
    const void *pc;
    void *ptr;
    size_t size;
    void *ret;
    void *(*func)(void *, size_t);
};

struct free_event {
    const void *pc;
    void *ptr;
    void (*func)(void *);
};

struct posix_memalign_event {
    const void *pc;
    void **ptr;
    size_t alignment;
    size_t size;
    int ret;
    int (*func)(void **, size_t, size_t);
};

struct aligned_alloc_event {
    const void *pc;
    size_t alignment;
    size_t size;
    void *ret;
    void *(*func)(size_t, size_t);
};

struct malloc_usable_size_event {
    const void *pc;
#if defined(__NetBSD__)
    const void *ptr;
#else
    void *ptr;
#endif
    size_t ret;
#if defined(__NetBSD__)
    size_t (*func)(const void *);
#else
    size_t (*func)(void *);
#endif
};

#endif /* DICE_MALLOC_H */
