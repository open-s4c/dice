/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 * -----------------------------------------------------------------------------
 * @file intercept.h
 * @brief Standard intercept chains.
 */
#ifndef DICE_INTERCEPT_H
#define DICE_INTERCEPT_H

#include <dice/compiler.h>

/* CHAIN_INTERCEPT_EVENT can be used to implement annotations, inform callbacks
 * about the execution, or replace system functions. */
#ifndef DICE_ROLLED
    #define CHAIN_INTERCEPT_EVENT 1
#endif

/* CHAIN_INTERCEPT_BEFORE can be used before a syscall is called, or before a
 * cmpxchg operation is performed, etc. If you publish CHAIN_INTERCEPT_BEFORE,
 * you are expected to publish CHAIN_INTERCEPT_AFTER subsequently. */
#ifndef DICE_ROLLED
    #define CHAIN_INTERCEPT_BEFORE 2
#endif

/* CHAIN_INTERCEPT_AFTER can be used after a syscall is called or after a
 * cmpxchg operation is performed, etc. If you publish CHAIN_INTERCEPT_AFTER,
 * you must have published CHAIN_INTERCEPT_BEFORE. You can use the same metadata
 * object on both publications. */
#ifndef DICE_ROLLED
    #define CHAIN_INTERCEPT_AFTER 3
#endif

/* Existing sources retain their public chain names. */
#ifndef INTERCEPT_EVENT
    #define INTERCEPT_EVENT CHAIN_INTERCEPT_EVENT
#endif
#ifndef INTERCEPT_BEFORE
    #define INTERCEPT_BEFORE CHAIN_INTERCEPT_BEFORE
#endif
#ifndef INTERCEPT_AFTER
    #define INTERCEPT_AFTER CHAIN_INTERCEPT_AFTER
#endif

#endif /* DICE_INTERCEPT_H */
