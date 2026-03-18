/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/cpuset_alloc.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(cpu_set_t *, __sched_cpualloc, size_t count)
{
    struct sched_cpualloc_event ev = {
        .pc    = INTERPOSE_PC,
        .count = count,
        .ret   = 0,
        .func  = REAL_FUNC(__sched_cpualloc),
    };

    struct metadata md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SCHED_CPUALLOC, &ev, &md);
    ev.ret = ev.func(ev.count);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SCHED_CPUALLOC, &ev, &md);
    return ev.ret;
}

PS_ADVERTISE_TYPE(EVENT_SCHED_CPUALLOC)

DICE_MODULE_INIT()
