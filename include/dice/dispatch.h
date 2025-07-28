/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_DISPATCH_H
#define DICE_DISPATCH_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

#define PS_DISPATCH(CHAIN, TYPE, SLOT)                                         \
    V_JOIN(V_JOIN(ps_dispatch, V_JOIN(CHAIN, V_JOIN(TYPE, SLOT))), )

#define PS_DISPATCH_DECL(CHAIN, TYPE, SLOT)                                    \
    DICE_HIDE enum ps_err PS_DISPATCH(CHAIN, TYPE, SLOT)(                      \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        return PS_HANDLER(CHAIN, TYPE, SLOT)(chain, type, event, md);          \
    }

DICE_HIDE bool
V_JOIN(V_JOIN(ps_dispatch, DICE_MODULE_PRIO), on_)(void)
{
    return true;
}

#endif /* DICE_DISPATCH_H */
