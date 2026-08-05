/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <stddef.h>

#include <dice/chains/intercept.h>
#include <dice/events/unistd.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <errno.h>

#define CHECK_OFFSET(FIELD, OFFSET)                                            \
    _Static_assert(offsetof(struct syscall_event, FIELD) == (OFFSET),          \
                   "unexpected syscall_event." #FIELD " offset")

CHECK_OFFSET(pc, 0);
CHECK_OFFSET(number, 8);
CHECK_OFFSET(args, 16);
CHECK_OFFSET(ret, 64);
CHECK_OFFSET(func, 80);
_Static_assert(sizeof(struct syscall_event) == 88,
               "unexpected syscall_event size");
_Static_assert(sizeof(metadata_t) == 8,
               "syscall asm metadata slot size must match Dice metadata_t");

REAL_DECL(int, syscall, long, ...);

DICE_HIDE void *
dice_syscall_before(struct syscall_event *ev, metadata_t *md)
{
    ev->func = REAL_FUNC(syscall);
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SYSCALL, ev, md);
    errno = 0;
    return (void *)ev->func;
}

DICE_HIDE void
dice_syscall_after(struct syscall_event *ev, metadata_t *md)
{
    ev->errno_ = errno;
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SYSCALL, ev, md);
    errno = ev->errno_;
}

PS_ADVERTISE_TYPE(EVENT_SYSCALL)

DICE_MODULE_INIT()
