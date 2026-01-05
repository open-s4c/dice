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

/* MAX_BUILTIN_SLOTS determines the maximum number of builtin slots. The range
 * 0..MAX_BUILTIN_SLOTS-1 is reserved for builtin modules. */
#ifndef MAX_BUILTIN_SLOTS
    #define MAX_BUILTIN_SLOTS 6
#endif
STATIC_ASSERT(MAX_BUILTIN_SLOTS > 0, "Slot 0 is always builtin");

#ifndef DICE_MODULE_SLOT
    /* Subscription slot for the current translation unit. Lower values run
     * first. Builtin modules reserve slots 0..MAX_BUILTIN_SLOTS-1; plugin
     * modules should stay above that range. */
    #define DICE_MODULE_SLOT 9999
#endif

/* Enable ps_dispatch if within the maximum number of builtin slots */
#if DICE_MODULE_SLOT < MAX_BUILTIN_SLOTS
    #include <dice/dispatch.h>
#else
    #define PS_DISPATCH_DEF(CHAIN, TYPE, SLOT)
#endif

/* DICE_MODULE_INIT wraps the constructor logic of a module. The callback runs
 * exactly once even if the module is linked multiple times (e.g., builtin plus
 * plugin builds). */
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
            log_debug("[%4d] INIT: %s", DICE_MODULE_SLOT, __FILE__);           \
    }                                                                          \
    PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_INIT, {                             \
        if (module_init_())                                                    \
            log_debug("[%4d] INIT! %s", DICE_MODULE_SLOT, __FILE__);           \
    })


/* DICE_MODULE_FINI marks a destructor hook. Use it for cleanup that must run
 * when the module is unloaded. */
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
 *
 * The PS_SUBSCRIBE macro also registers the chain and type names in the pubsub.
 * This is helpful for debugging.
 */
#define PS_SUBSCRIBE(CHAIN, TYPE, HANDLER)                                     \
    PS_SUBSCRIBE_SLOT(CHAIN, #CHAIN, TYPE, #TYPE, DICE_MODULE_SLOT, HANDLER)

#define PS_SUBSCRIBE_SLOT(CHAIN, CNAME, TYPE, TNAME, SLOT, HANDLER)            \
    PS_HANDLER_DEF(CHAIN, TYPE, SLOT, HANDLER)                                 \
    PS_DISPATCH_DEF(CHAIN, TYPE, SLOT)                                         \
    static void DICE_CTOR ps_subscribe_##CHAIN##_##TYPE##_(void)               \
    {                                                                          \
        ps_register_chain(CHAIN, CNAME);                                       \
        int err =                                                              \
            ps_subscribe(CHAIN, TYPE, PS_HANDLER(CHAIN, TYPE, SLOT), SLOT);    \
        if (err != PS_OK)                                                      \
            log_fatal("could not subscribe %s_%s_%u: %d", #CHAIN, #TYPE, SLOT, \
                      err);                                                    \
    }

/* PS_ADVERTISE_CHAIN macro registers the chain name in the pubsub. This is
 * optional and only helpful for debugging.
 */
#define PS_ADVERTISE_CHAIN(CHAIN)                                              \
    static void DICE_CTOR ps_advertise_chain_##CHAIN##_(void)                  \
    {                                                                          \
        ps_register_chain(CHAIN, #CHAIN);                                      \
    }

/* PS_ADVERTISE_TYPE macro registers the event type name in the pubsub registry.
 * This is optional and only helpful for debugging.
 */
#define PS_ADVERTISE_TYPE(TYPE)                                                \
    static void DICE_CTOR ps_advertise_type_##TYPE##_(void)                    \
    {                                                                          \
        ps_register_type(TYPE, #TYPE);                                         \
    }

#endif /* DICE_MODULE_H */
