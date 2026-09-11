/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/dirent.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(int, closedir, DIR *dirp)
{
    struct closedir_event ev = {
        .pc   = INTERPOSE_PC,
        .dirp = dirp,
        .ret  = 0,
        .func = REAL_FUNC(closedir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOSEDIR, &ev, &md);
    ev.ret = ev.func(ev.dirp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOSEDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(DIR *, fdopendir, int fd)
{
    struct fdopendir_event ev = {
        .pc   = INTERPOSE_PC,
        .fd   = fd,
        .ret  = NULL,
        .func = REAL_FUNC(fdopendir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_FDOPENDIR, &ev, &md);
    ev.ret = ev.func(ev.fd);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_FDOPENDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(DIR *, opendir, const char *name)
{
    struct opendir_event ev = {
        .pc   = INTERPOSE_PC,
        .name = name,
        .ret  = NULL,
        .func = REAL_FUNC(opendir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_OPENDIR, &ev, &md);
    ev.ret = ev.func(ev.name);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_OPENDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(struct dirent *, readdir, DIR *dirp)
{
    struct readdir_event ev = {
        .pc   = INTERPOSE_PC,
        .dirp = dirp,
        .ret  = NULL,
        .func = REAL_FUNC(readdir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READDIR, &ev, &md);
    ev.ret = ev.func(ev.dirp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READDIR, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, readdir_r, DIR *restrict dirp,
          struct dirent *restrict entry,
          struct dirent **restrict result)
{
    struct readdir_r_event ev = {
        .pc     = INTERPOSE_PC,
        .dirp   = dirp,
        .entry  = entry,
        .result = result,
        .ret    = 0,
        .func   = REAL_FUNC(readdir_r),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_READDIR_R, &ev, &md);
    ev.ret = ev.func(ev.dirp, ev.entry, ev.result);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_READDIR_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(void, rewinddir, DIR *dirp)
{
    struct rewinddir_event ev = {
        .pc   = INTERPOSE_PC,
        .dirp = dirp,
        .func = REAL_FUNC(rewinddir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REWINDDIR, &ev, &md);
    ev.func(ev.dirp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REWINDDIR, &ev, &md);
}

INTERPOSE(void, seekdir, DIR *dirp, long loc)
{
    struct seekdir_event ev = {
        .pc   = INTERPOSE_PC,
        .dirp = dirp,
        .loc  = loc,
        .func = REAL_FUNC(seekdir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SEEKDIR, &ev, &md);
    ev.func(ev.dirp, ev.loc);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SEEKDIR, &ev, &md);
}

INTERPOSE(long, telldir, DIR *dirp)
{
    struct telldir_event ev = {
        .pc   = INTERPOSE_PC,
        .dirp = dirp,
        .ret  = 0,
        .func = REAL_FUNC(telldir),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TELLDIR, &ev, &md);
    ev.ret = ev.func(ev.dirp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TELLDIR, &ev, &md);
    return ev.ret;
}

/* Advertise event types */
PS_ADVERTISE_TYPE(EVENT_CLOSEDIR)
PS_ADVERTISE_TYPE(EVENT_FDOPENDIR)
PS_ADVERTISE_TYPE(EVENT_OPENDIR)
PS_ADVERTISE_TYPE(EVENT_READDIR)
PS_ADVERTISE_TYPE(EVENT_READDIR_R)
PS_ADVERTISE_TYPE(EVENT_REWINDDIR)
PS_ADVERTISE_TYPE(EVENT_SEEKDIR)
PS_ADVERTISE_TYPE(EVENT_TELLDIR)

/* Module init (optional) */
DICE_MODULE_INIT()
