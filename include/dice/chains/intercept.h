/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 * -----------------------------------------------------------------------------
 * @file intercept.h
 * @brief Standard intercept chains.
 */
#ifndef DICE_INTERCEPT_H
#define DICE_INTERCEPT_H

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

#endif /* DICE_INTERCEPT_H */
