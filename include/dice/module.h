/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file module.h
 * @brief Dice module helpers
 *
 * This header defines the minimal set of macros and declares a few common
 * prototypes used in Dice modules. Typically a module has an initialization
 * constructor defined as:
 *
 *    DICE_MODULE_INIT({
 *       // do some initialization
 *     })
 */
#ifndef DICE_MODULE_H
#define DICE_MODULE_H
#include <dice/compiler.h>
#include <dice/events/dice.h>
#include <dice/log.h>
#include <dice/pubsub.h>

#define Y(V) #V
#define X(V) Y(V)

#define CHAIN_CONTROL 0

#define DICE_MODULE_INIT(CODE)                                                 \
    static bool _module_init()                                                 \
    {                                                                          \
        static bool _done = false;                                             \
        if (!_done) {                                                          \
            _done = true;                                                      \
            do {                                                               \
                CODE                                                           \
            } while (0);                                                       \
            return true;                                                       \
        }                                                                      \
        return false;                                                          \
    }                                                                          \
    static DICE_CTOR void _module_ctr()                                        \
    {                                                                          \
        if (_module_init())                                                    \
            log_printf("LOADED[" X(DICE_XTOR_PRIO) "] %s\n", __FILE__);        \
    }                                                                          \
    PS_SUBSCRIBO(CHAIN_CONTROL, EVENT_DICE_INIT, {                             \
        if (_module_init())                                                    \
            log_printf("FORCE LOADED[" X(DICE_XTOR_PRIO) "] %s\n", __FILE__);  \
    })


#define DICE_MODULE_FINI(CODE)                                                 \
    static DICE_DTOR void _module_fini()                                       \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }

#endif /* DICE_MODULE_H */
