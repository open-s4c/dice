/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file module.h
 * @brief Bingo module helpers
 *
 * This header defines the minimal set of macros and declares a few common
 * prototypes used in Bingo modules. Typically a module has an initialization
 * constructor defined as:
 *
 *    BINGO_MODULE_INIT({
 *       // do some initialization
 *     })
 */
#ifndef BINGO_MODULE_H
#define BINGO_MODULE_H
#include <bingo/compiler.h>
#include <bingo/log.h>

#define Y(V) #V
#define X(V) Y(V)

#define BINGO_MODULE_INIT(CODE)                                                \
    static BINGO_CTOR void _module_init()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
        log_printf("LOADED[" X(BINGO_XTOR_PRIO) "] %s\n", __FILE__);           \
    }

#define BINGO_MODULE_FINI(CODE)                                                \
    static BINGO_DTOR void _module_fini()                                      \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }

void bingo_init(void);
void bingo_fini(void);

#endif /* BINGO_MODULE_H */
