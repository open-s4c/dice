/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_CPUSET_ALLOC_H
#define DICE_CPUSET_ALLOC_H

#include <sched.h>

#define EVENT_SCHED_CPUALLOC 69

struct sched_cpualloc_event {
    const void *pc;
    size_t count;
    cpu_set_t *ret;
    cpu_set_t *(*func)(size_t);
};

#endif /* DICE_CPUSET_ALLOC_H */
