/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_DISPATCH_H
#define DICE_DISPATCH_H
#include <dice/compiler.h>
#include <dice/pubsub.h>

/* NOTE: Do not include this file or call its macros. Instead, please
 * include dice/module.h and use the macros provided there. */

/* PS_DISPATCH builds the canonical name of dispatchers associated with a
 * given chain/type/slot triple. The resulting name  follows the pattern
 * ps_dispatch_CHAIN_TYPE_SLOT_, where CHAIN, TYPE and SLOT are resolved to
 * their underlying numbers. */
#define PS_DISPATCH(CHAIN, TYPE, SLOT)                                         \
    V_JOIN(V_JOIN(ps_dispatch, V_JOIN(CHAIN, V_JOIN(TYPE, SLOT))), )

/* PS_DISPATCH_DEF defines a dispatcher that calls the PS_HANDLER function,
 * which in turn contains the user code. */
#define PS_DISPATCH_DEF(CHAIN, TYPE, SLOT)                                     \
    DICE_HIDE enum ps_err PS_DISPATCH(CHAIN, TYPE, SLOT)(                      \
        const chain_id chain, const type_id type, void *event,                 \
        struct metadata *md)                                                   \
    {                                                                          \
        return PS_HANDLER(CHAIN, TYPE, SLOT)(chain, type, event, md);          \
    }

/* Defines ps_dispatch_slot_SLOT_on_, where SLOT is a number. The dispatcher
 * module calls this function to determine if the main Dice library has a
 * builtin module for this slot. It overwrites a weakly defined symbol in the
 * dispatcher module, which always return false. */
#define PS_DISPATCH_SLOT_ON(SLOT)                                              \
    DICE_HIDE bool V_JOIN(V_JOIN(ps_dispatch_slot, SLOT), on_)(void)           \
    {                                                                          \
        return true;                                                           \
    }

#ifndef DICE_MULTIFILE_MODULE

PS_DISPATCH_SLOT_ON(DICE_MODULE_SLOT)

    #undef PS_DISPATCH_SLOT_ON
    #define PS_DISPATCH_SLOT_ON(_)
#endif

#endif /* DICE_DISPATCH_H */
