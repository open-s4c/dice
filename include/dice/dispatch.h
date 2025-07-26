/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file dispatch.h
 * @brief Dispatch macros
 */
#ifndef DICE_DISPATCH_H
#define DICE_DISPATCH_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

#define PS_DISPATCH(CHAIN, TYPE, SLOT)                                         \
    V_JOIN(V_JOIN(ps_callback, V_JOIN(CHAIN, V_JOIN(TYPE, SLOT))), )

#ifdef DICE_BUILTIN
    #define PS_DISPATCH_DECL(CHAIN, TYPE, SLOT)                                \
        DICE_HIDE enum ps_cb_err PS_DISPATCH(CHAIN, TYPE, SLOT)(               \
            const chain_id chain, const type_id type, void *event,             \
            metadata_t *md)                                                    \
        {                                                                      \
            return PS_CALLBACK(CHAIN, TYPE, SLOT)(chain, type, event, md);     \
        }
#else
    #define PS_DISPATCH_DECL(CHAIN, TYPE, SLOT)
#endif

#endif /* DICE_DISPATCH_H */
