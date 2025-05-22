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
#include <dice/log.h>

#define Y(V) #V
#define X(V) Y(V)

#define DICE_MODULE_INIT(CODE)                                                \
    static DICE_CTOR void _module_init()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
        log_printf("LOADED[" X(DICE_XTOR_PRIO) "] %s\n", __FILE__);           \
    }

#define DICE_MODULE_FINI(CODE)                                                \
    static DICE_DTOR void _module_fini()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }

void dice_init(void);
void dice_fini(void);

#endif /* DICE_MODULE_H */
