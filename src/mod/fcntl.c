/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <dice/chains/intercept.h>
#include <dice/events/fcntl.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <stdarg.h>

INTERPOSE(int, creat, const char *path, mode_t mode)
{
    struct creat_event ev = {
        .pc   = INTERPOSE_PC,
        .path = path,
        .mode = mode,
        .ret  = 0,
        .func = REAL_FUNC(creat),
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CREAT, &ev, &md);
    ev.ret = ev.func(ev.path, ev.mode);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CREAT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, fcntl, int fd, int cmd, ...)
{
    uint64_t arg;
    va_list ap;
    va_start(ap, cmd);
    switch (cmd) {
        case F_GETFD:
        case F_GETFL:
        case F_SETFD:
        case F_SETFL:
        case F_SETOWN:
#if !defined(__APPLE__)
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETSIG:
        case F_NOTIFY:
        case F_ADD_SEALS:
        case F_SETPIPE_SZ:
#endif
            arg = va_arg(ap, int);
            break;
#if !defined(__APPLE__)
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
        case F_OFD_GETLK:
        case F_OFD_SETLK:
        case F_OFD_SETLKW:
            arg = (uint64_t)(uintptr_t)va_arg(ap, struct flock *);
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
             arg = (uint64_t)(uintptr_t)va_arg(ap, struct f_owner_ex *);
             break;
        case F_GET_RW_HINT:
        case F_SET_RW_HINT:
        case F_GET_FILE_RW_HINT:
        case F_SET_FILE_RW_HINT:
            arg = va_arg(ap, uint64_t);
            break;
#endif
        default:
            assert(0 && "Unsupported fcntl command");
            arg = 0;
            break;
    }
    va_end(ap);

    struct fcntl_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .cmd  = cmd,
        .arg  = arg,
        .ret  = 0,
        .func = REAL_FUNC(fcntl),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FCNTL, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.cmd, ev.arg);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FCNTL, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, open, const char *path, int flags, ...)
{
    va_list ap;
    mode_t mode = 0;
    if ((flags & O_CREAT) || (flags & O_TMPFILE)) {
        va_start(ap, flags);
#ifdef __APPLE__
        int promoted_mode = va_arg(ap, int);
        mode = (mode_t)promoted_mode;
#else
        mode = va_arg(ap, mode_t);
#endif
        va_end(ap);
    }

    struct open_event ev = {
        .pc    = INTERPOSE_PC,
        .path  = path,
        .flags = flags,
        .mode  = mode,
        .ret   = 0,
        .func  = REAL_FUNC(open),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_OPEN, &ev, &md);
    ev.ret = ev.func(ev.path, ev.flags, ev.mode);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_OPEN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, openat, int dirfd, const char *path, int flags, ...)
{
    va_list ap;
    mode_t mode = 0;
    if ((flags & O_CREAT) || (flags & O_TMPFILE)) {
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }

    struct openat_event ev = {
        .pc     = INTERPOSE_PC,
        .dirfd  = dirfd,
        .path   = path,
        .flags  = flags,
        .mode   = mode,
        .ret    = 0,
        .func   = REAL_FUNC(openat),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_OPENAT, &ev, &md);
    ev.ret = ev.func(ev.dirfd, ev.path, ev.flags, ev.mode);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_OPENAT, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, openat2, int dirfd, const char *path, const struct open_how *how, size_t size)
{
    struct openat2_event ev = {
        .pc    = INTERPOSE_PC,
        .dirfd = dirfd,
        .path  = path,
        .how   = how,
        .size  = size,
        .ret   = 0,
        .func  = REAL_FUNC(openat2),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_OPENAT2, &ev, &md);
    ev.ret = ev.func(ev.dirfd, ev.path, ev.how, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_OPENAT2, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, posix_fadvise, int fd, off_t offset, off_t size, int advice)
{
    struct posix_fadvise_event ev = {
        .pc     = INTERPOSE_PC,
        .fd     = fd,
        .offset = offset,
        .size   = size,
        .advice = advice,
        .ret    = 0,
        .func   = REAL_FUNC(posix_fadvise),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_FADVISE, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.offset, ev.size, ev.advice);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POSIX_FADVISE, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(int, posix_fallocate, int fd, off_t offset, off_t size)
{
    struct posix_fallocate_event ev = {
        .pc     = INTERPOSE_PC,
        .fd     = fd,
        .offset = offset,
        .size   = size,
        .ret    = 0,
        .func   = REAL_FUNC(posix_fallocate),
    };
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_POSIX_FALLOCATE, &ev, &md);
    ev.ret = ev.func(ev.fd, ev.offset, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_POSIX_FALLOCATE, &ev, &md);
    return ev.ret;
}

/* Advertise event types */
PS_ADVERTISE_TYPE(EVENT_CREAT)
PS_ADVERTISE_TYPE(EVENT_FCNTL)
PS_ADVERTISE_TYPE(EVENT_OPEN)
PS_ADVERTISE_TYPE(EVENT_OPENAT)
PS_ADVERTISE_TYPE(EVENT_OPENAT2)
PS_ADVERTISE_TYPE(EVENT_POSIX_FADVISE)
PS_ADVERTISE_TYPE(EVENT_POSIX_FALLOCATE)

/* Module init (optional) */
DICE_MODULE_INIT()
