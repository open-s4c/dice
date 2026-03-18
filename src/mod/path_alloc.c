/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/path_alloc.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(char *, get_current_dir_name, void)
{
    struct get_current_dir_name_event ev = {
        .pc   = INTERPOSE_PC,
        .ret  = 0,
        .func = REAL_FUNC(get_current_dir_name),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GET_CURRENT_DIR_NAME, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GET_CURRENT_DIR_NAME, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, getcwd, char *buf, size_t size)
{
    struct getcwd_event ev = {
        .pc   = INTERPOSE_PC,
        .buf  = buf,
        .size = size,
        .ret  = 0,
        .func = REAL_FUNC(getcwd),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETCWD, &ev, &md);
    ev.ret = ev.func(ev.buf, ev.size);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETCWD, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, realpath, const char *path, char *resolved_path)
{
    struct realpath_event ev = {
        .pc            = INTERPOSE_PC,
        .path          = path,
        .resolved_path = resolved_path,
        .ret           = 0,
        .func          = REAL_FUNC(realpath),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_REALPATH, &ev, &md);
    ev.ret = ev.func(ev.path, ev.resolved_path);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_REALPATH, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, tempnam, const char *dir, const char *pfx)
{
    struct tempnam_event ev = {
        .pc   = INTERPOSE_PC,
        .dir  = dir,
        .pfx  = pfx,
        .ret  = 0,
        .func = REAL_FUNC(tempnam),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TEMPNAM, &ev, &md);
    ev.ret = ev.func(ev.dir, ev.pfx);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TEMPNAM, &ev, &md);
    return ev.ret;
}

PS_ADVERTISE_TYPE(EVENT_GET_CURRENT_DIR_NAME)
PS_ADVERTISE_TYPE(EVENT_GETCWD)
PS_ADVERTISE_TYPE(EVENT_REALPATH)
PS_ADVERTISE_TYPE(EVENT_TEMPNAM)

DICE_MODULE_INIT()
