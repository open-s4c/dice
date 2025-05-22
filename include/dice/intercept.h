/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file intercept.h
 * @brief Standard intercept chains and helpers.
 */
#ifndef DICE_INTERCEPT_H
#define DICE_INTERCEPT_H

#include <dice/compiler.h>
#include <dice/pubsub.h>

/* INTERCEPT_EVENT can be used to implement annotations, inform callbacks about
 * the execution, or replace system functions. */
#define INTERCEPT_EVENT 1

/* INTERCEPT_BEFORE can be used before a syscall is called, or before a cmpxchg
 * operation is performed, etc. If you publish INTERCEPT_BEFORE, you are
 * expected to publish INTERCEPT_AFTER subsequently. */
#define INTERCEPT_BEFORE 2

/* INTERCEPT_AFTER can be used after a syscall is called or after a cmpxchg
 * operation is performed, etc. If you publish INTERCEPT_AFTER, you must have
 * published INTERCEPT_BEFORE. You can use the same metadata object on both
 * publications. */
#define INTERCEPT_AFTER 3

/* EVENT_PAYLOAD casts the event argument `event` to type of the given
 * variable.
 *
 * This macro is intended to be used with REGISTER_CALLBACK. The user must
 * know the type of the argument and then the following pattern can be used:
 *
 *     REGISTER_CALLBACK(SOME_HOOK, SOME_EVENT, {
 *         some_known_type *ev = EVENT_PAYLOAD(ev);
 *         ...
 *         })
 */
#define EVENT_PAYLOAD(var) (__typeof(var))event

#define INTERPOSE_PC                                                           \
    (__builtin_extract_return_addr(__builtin_return_address(0)))

#endif /* DICE_INTERCEPT_H */
