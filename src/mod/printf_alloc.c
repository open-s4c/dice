/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stdarg.h>

#include <dice/chains/intercept.h>
#include <dice/events/printf_alloc.h>
#include <dice/interpose.h>
#include <dice/module.h>

#ifdef HAVE_VASPRINTF
INTERPOSE(int, vasprintf, char **strp, const char *fmt, va_list ap)
{
    struct vasprintf_event ev = {
        .pc   = INTERPOSE_PC,
        .strp = strp,
        .fmt  = fmt,
        .ret  = -1,
        .func = REAL_FUNC(vasprintf),
    };

    va_copy(ev.ap, ap);

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_VASPRINTF, &ev, &md);
    ev.ret = ev.func(ev.strp, ev.fmt, ev.ap);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_VASPRINTF, &ev, &md);
    va_end(ev.ap);
    return ev.ret;
}
#endif

#ifdef HAVE_ASPRINTF
INTERPOSE(int, asprintf, char **strp, const char *fmt, ...)
{
    struct asprintf_event ev = {
        .pc    = INTERPOSE_PC,
        .strp  = strp,
        .fmt   = fmt,
        .ret   = -1,
        .func  = REAL_FUNC(asprintf),
        .vfunc = REAL_FUNC(vasprintf),
    };
    va_list ap;

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ASPRINTF, &ev, &md);
    va_start(ap, fmt);
    ev.ret = ev.vfunc(ev.strp, ev.fmt, ap);
    va_end(ap);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ASPRINTF, &ev, &md);
    return ev.ret;
}
#endif

#ifdef HAVE_ASPRINTF
PS_ADVERTISE_TYPE(EVENT_ASPRINTF)
#endif
#ifdef HAVE_VASPRINTF
PS_ADVERTISE_TYPE(EVENT_VASPRINTF)
#endif

DICE_MODULE_INIT()
