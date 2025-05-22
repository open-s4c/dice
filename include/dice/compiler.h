/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_COMPILER_H
#define DICE_COMPILER_H

#include <vsync/atomic.h>

#ifndef DICE_XTOR_PRIO
    #define DICE_XTOR_PRIO
#endif

#define DICE_CTOR  __attribute__((constructor(DICE_XTOR_PRIO)))
#define DICE_DTOR  __attribute__((destructor(DICE_XTOR_PRIO)))
#define DICE_WEAK  __attribute__((weak))
#define DICE_NORET _Noreturn

#ifndef DICE_NOHIDE
    #define DICE_HIDE __attribute__((visibility("hidden")))
#else
    #define DICE_HIDE
#endif

#ifndef likely
    #define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
    #define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#endif /* DICE_COMPILER_H */
