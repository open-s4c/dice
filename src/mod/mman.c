/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/mman.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <stdarg.h>

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

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MMAP, &ev, &md);
    ev.ret = ev.func(ev.addr, ev.length, ev.prot, ev.flags, ev.fd, ev.offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MMAP, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__) && !defined(__OHOS__)
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

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MMAP, &ev, &md);
    ev.ret = ev.func(ev.addr, ev.length, ev.prot, ev.flags, ev.fd, ev.offset);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MMAP, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, munmap, void *addr, size_t length)
{
    struct munmap_event ev = {
        .pc     = INTERPOSE_PC,
        .addr   = addr,
        .length = length,
        .ret    = 0,
        .func   = REAL_FUNC(munmap),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MUNMAP, &ev, &md);
    ev.ret = ev.func(ev.addr, ev.length);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MUNMAP, &ev, &md);
    return ev.ret;
}

INTERPOSE(void *, mremap,
          void *old_address,
          size_t old_size,
          size_t new_size,
          int flags,
          ...)
{
    va_list ap;
    va_start(ap, flags);
    void *new_address = va_arg(ap, void *);
    struct mremap_event ev = {
        .pc          = INTERPOSE_PC,
        .old_address = old_address,
        .old_size    = old_size,
        .new_size    = new_size,
        .flags       = flags,
        .new_address = new_address,
        .ret         = NULL,
        .func        = REAL_FUNC(mremap),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MREMAP, &ev, &md);
    ev.ret = ev.func(
        ev.old_address,
        ev.old_size,
        ev.new_size,
        ev.flags,
        ev.new_address);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MREMAP, &ev, &md);

    return ev.ret;
}

INTERPOSE(int, mprotect, void *addr, size_t length, int prot)
{
    struct mprotect_event ev = {
        .pc     = INTERPOSE_PC,
        .addr   = addr,
        .length = length,
        .prot   = prot,
        .ret    = 0,
        .func   = REAL_FUNC(mprotect),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MPROTECT, &ev, &md);
    ev.ret = ev.func(ev.addr, ev.length, ev.prot);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MPROTECT, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_MMAP)
PS_ADVERTISE_TYPE(EVENT_MUNMAP)
PS_ADVERTISE_TYPE(EVENT_MREMAP)
PS_ADVERTISE_TYPE(EVENT_MPROTECT)
/* Mark module initialization (optional) */
DICE_MODULE_INIT()
