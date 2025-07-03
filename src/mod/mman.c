/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
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
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MMAP, &ev, &md);
    ev.ret = REAL(mmap, addr, length, prot, flags, fd, offset);
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
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUNMAP, &ev, &md);
    ev.ret = REAL(munmap, addr, length);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUNMAP, &ev, &md);
    return ev.ret;
}

DICE_MODULE_INIT()
