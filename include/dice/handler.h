/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_HANDLER_H
#define DICE_HANDLER_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

/* NOTE: Do not include this file or call its macros. Instead, please include
 * dice/module.h and use the macros provided there. */

/* PS_HANDLER builds the canonical name for handlers associated with a given
 * chain/type/priority triple. The resulting name follows the pattern
 * ps_handler_CHAIN_TYPE_SLOT, where CHAIN, TYPE and SLOT are resolved to their
 * underlying numbers. */
#define PS_HANDLER(CHAIN, TYPE, SLOT)                                          \
    V_JOIN(V_JOIN(V_JOIN(ps_handler, CHAIN), TYPE), SLOT)

/* PS_HANDLER_DEF defines an inline handler that wraps user code and returns a
 * ps_err. */
#define PS_HANDLER_DEF(CHAIN, TYPE, SLOT, CODE)                                \
    static inline enum ps_err PS_HANDLER(CHAIN, TYPE, SLOT)(                   \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        /* Parameters are marked as unused to silence warnings. */             \
        /* Nevertheless, the callback can use parameters without issues. */    \
        (void)chain;                                                           \
        (void)type;                                                            \
        (void)event;                                                           \
        (void)md;                                                              \
                                                                               \
        CODE;                                                                  \
                                                                               \
        /* By default, callbacks return OK to continue chain publishing. */    \
        return PS_OK;                                                          \
    }

#endif /* DICE_HANDLER_H */
