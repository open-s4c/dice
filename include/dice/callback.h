/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file callback.h
 * @brief Callback macros
 */
#ifndef DICE_CALLBACK_H
#define DICE_CALLBACK_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

#define PS_CALLBACK(CHAIN, TYPE, SLOT)                                         \
    V_JOIN(V_JOIN(V_JOIN(ps_callback, CHAIN), TYPE), SLOT)

#define PS_CALLBACK_DECL(CHAIN, TYPE, SLOT, CALLBACK)                          \
    static inline enum ps_cb_err PS_CALLBACK(CHAIN, TYPE, SLOT)(               \
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
        CALLBACK;                                                              \
                                                                               \
        /* By default, callbacks return OK to continue chain publishing.       \
         */                                                                    \
        return PS_CB_OK;                                                       \
    }

#endif /* DICE_CALLBACK_H */
