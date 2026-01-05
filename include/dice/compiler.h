/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_COMPILER_H
#define DICE_COMPILER_H

#include <stddef.h>

#ifndef DICE_XTOR_PRIO
    #define DICE_XTOR_PRIO
#endif

#define DICE_CTOR   __attribute__((constructor(DICE_XTOR_PRIO)))
#define DICE_DTOR   __attribute__((destructor(DICE_XTOR_PRIO)))
#define DICE_WEAK   __attribute__((weak))
#define DICE_NORET  _Noreturn
#define DICE_UNUSED __attribute__((unused))

#ifndef DICE_NOHIDE
    #define DICE_HIDE __attribute__((visibility("hidden")))
#else
    #define DICE_HIDE
#endif

#ifdef DICE_HIDE_ALL
    #define DICE_HIDE_IF __attribute__((visibility("hidden")))
#else
    #define DICE_HIDE_IF DICE_WEAK
#endif

#ifndef likely
    #define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
    #define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#ifndef STATIC_ASSERT
    #if defined(__cplusplus)
        #define STATIC_ASSERT(c, m) static_assert(c, m)
    #else
        #define STATIC_ASSERT(c, m) _Static_assert(c, m)
    #endif
#endif

#endif /* DICE_COMPILER_H */
