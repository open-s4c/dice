/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
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
#include <dice/handler.h>
#include <dice/log.h>
#include <dice/pubsub.h>


#ifndef DICE_MODULE_PRIO
    #define DICE_MODULE_PRIO 9999
#endif

// should enable ps_dispatch?
#if DICE_MODULE_PRIO < 16
    #include <dice/dispatch.h>
#else
    #define PS_DISPATCH_DECL(CHAIN, TYPE, SLOT)
#endif

#define DICE_MODULE_INIT(CODE)                                                 \
    static bool module_init_()                                                 \
    {                                                                          \
        static bool done_ = false;                                             \
        if (!done_) {                                                          \
            done_ = true;                                                      \
            do {                                                               \
                CODE                                                           \
            } while (0);                                                       \
            return true;                                                       \
        }                                                                      \
        return false;                                                          \
    }                                                                          \
    static DICE_CTOR void module_ctr_()                                        \
    {                                                                          \
        if (module_init_())                                                    \
            log_debug("[%4d] INIT: %s", DICE_MODULE_PRIO, __FILE__);           \
    }                                                                          \
    PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_INIT, {                             \
        if (module_init_())                                                    \
            log_debug("[%4d] INIT! %s", DICE_MODULE_PRIO, __FILE__);           \
    })


#define DICE_MODULE_FINI(CODE)                                                 \
    static DICE_DTOR void module_fini_()                                       \
    {                                                                          \
        if (1) {                                                               \
            CODE                                                               \
        }                                                                      \
    }

/* PS_SUBSCRIBE macro creates a callback handler and subscribes to a
 * chain.
 *
 * On load time, a constructor function registers the handler to the
 * chain. The order in which modules are loaded must be considered when
 * planning for the relation between handlers. The order is either given
 * by linking order (if compilation units are linked together) or by the
 * order of shared libraries in LD_PRELOAD.
 */
#define PS_SUBSCRIBE_SLOT(CHAIN, TYPE, SLOT, HANDLER)                          \
    PS_HANDLER_DECL(CHAIN, TYPE, SLOT, HANDLER)                                \
    PS_DISPATCH_DECL(CHAIN, TYPE, SLOT)                                        \
    static void DICE_CTOR ps_subscribe_##CHAIN##_##TYPE##_(void)               \
    {                                                                          \
        int err =                                                              \
            ps_subscribe(CHAIN, TYPE, PS_HANDLER(CHAIN, TYPE, SLOT), SLOT);    \
        if (err != PS_OK)                                                      \
            log_fatal("could not subscribe %s_%s_%u: %d", #CHAIN, #TYPE, SLOT, \
                      err);                                                    \
    }

#define PS_SUBSCRIBE(CHAIN, TYPE, HANDLER)                                     \
    PS_SUBSCRIBE_SLOT(CHAIN, TYPE, DICE_MODULE_PRIO, HANDLER)

#endif /* DICE_MODULE_H */
