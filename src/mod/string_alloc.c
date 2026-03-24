/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <string.h>

#include <dice/chains/intercept.h>
#include <dice/events/string_alloc.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(char *, strdup, const char *s)
{
    struct strdup_event ev = {
        .pc   = INTERPOSE_PC,
        .s    = s,
        .ret  = 0,
        .func = REAL_FUNC(strdup),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_STRDUP, &ev, &md);
    ev.ret = ev.func(ev.s);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_STRDUP, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, strndup, const char *s, size_t n)
{
    struct strndup_event ev = {
        .pc   = INTERPOSE_PC,
        .s    = s,
        .n    = n,
        .ret  = 0,
        .func = REAL_FUNC(strndup),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_STRNDUP, &ev, &md);
    ev.ret = ev.func(ev.s, ev.n);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_STRNDUP, &ev, &md);
    return ev.ret;
}

#ifdef HAVE_WCSDUP
INTERPOSE(wchar_t *, wcsdup, const wchar_t *s)
{
    struct wcsdup_event ev = {
        .pc   = INTERPOSE_PC,
        .s    = s,
        .ret  = 0,
        .func = REAL_FUNC(wcsdup),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_WCSDUP, &ev, &md);
    ev.ret = ev.func(ev.s);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_WCSDUP, &ev, &md);
    return ev.ret;
}
#endif

PS_ADVERTISE_TYPE(EVENT_STRDUP)
PS_ADVERTISE_TYPE(EVENT_STRNDUP)
#ifdef HAVE_WCSDUP
PS_ADVERTISE_TYPE(EVENT_WCSDUP)
#endif

DICE_MODULE_INIT()
