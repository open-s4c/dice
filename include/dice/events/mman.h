/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_MMAN_H
#define DICE_MMAN_H

#include <stddef.h>

#include <sys/mman.h>

#define EVENT_MMAP     80
#define EVENT_MUNMAP   81
#define EVENT_MREMAP   259
#define EVENT_MPROTECT 260

struct mmap_event {
    const void *pc;
    void *addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    off_t offset;
    void *ret;
    void *(*func)(void *, size_t, int, int, int, off_t);
};

struct munmap_event {
    const void *pc;
    void *addr;
    size_t length;
    int ret;
    int (*func)(void *, size_t);
};

struct mremap_event {
    const void *pc;
    void *old_address;
    size_t old_size;
    size_t new_size;
    int flags;
    void *new_address;
    void * ret;
    void *(*func)(void *, size_t, size_t, int, ...);
};

struct mprotect_event {
    const void *pc;
    void *addr;
    size_t length;
    int prot;
    int ret;
    int (*func)(void *, size_t, int);
};

#endif /* DICE_MMAN_H */
