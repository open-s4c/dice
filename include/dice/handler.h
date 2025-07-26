/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_HANDLER_H
#define DICE_HANDLER_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

#define PS_HANDLER(CHAIN, TYPE, SLOT)                                          \
    V_JOIN(V_JOIN(V_JOIN(ps_handler, CHAIN), TYPE), SLOT)

#define PS_HANDLER_DECL(CHAIN, TYPE, SLOT, CODE)                               \
    static inline enum ps_err PS_HANDLER(CHAIN, TYPE, SLOT)(                   \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        /* Parameters are marked as unused to silence warnings. */             \
        /* Nevertheless, the callback can use parameters without issues.       \
         */                                                                    \
        (void)chain;                                                           \
        (void)type;                                                            \
        (void)event;                                                           \
        (void)md;                                                              \
                                                                               \
        CODE;                                                                  \
                                                                               \
        /* By default, callbacks return OK to continue chain publishing.       \
         */                                                                    \
        return PS_OK;                                                          \
    }

#endif /* DICE_HANDLER_H */
