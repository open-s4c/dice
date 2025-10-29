/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/mman.h>
#include <dice/interpose.h>
#include <dice/pubsub.h>

INTERPOSE(void *, mmap, void *addr, size_t length, int prot, int flags, int fd,
          off_t offset)
{
    struct mmap_event ev = {
        .pc     = INTERPOSE_PC,
        .addr   = addr,
        .length = length,
        .prot   = prot,
        .flags  = flags,
        .fd     = fd,
        .offset = offset,
        .ret    = 0,
        .func   = REAL_FUNC(mmap),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MMAP, &ev, &md);
    ev.ret = ev.func(addr, length, prot, flags, fd, offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MMAP, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, mmap64, void *addr, size_t length, int prot, int flags,
          int fd, off_t offset)
{
    struct mmap_event ev = {
        .pc     = INTERPOSE_PC,
        .addr   = addr,
        .length = length,
        .prot   = prot,
        .flags  = flags,
        .fd     = fd,
        .offset = offset,
        .ret    = 0,
        .func   = REAL_FUNC(mmap64),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MMAP, &ev, &md);
    ev.ret = ev.func(addr, length, prot, flags, fd, offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MMAP, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, munmap, void *addr, size_t length)
{
    struct munmap_event ev = {
        .pc     = INTERPOSE_PC,
        .addr   = addr,
        .length = length,
        .ret    = 0,
        .func   = REAL_FUNC(munmap),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUNMAP, &ev, &md);
    ev.ret = ev.func(addr, length);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUNMAP, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_MMAP)
PS_ADVERTISE_TYPE(EVENT_MUNMAP)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
