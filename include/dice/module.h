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
#include <dice/events/dice.h>
#include <dice/log.h>
#include <dice/pubsub.h>

#define CHAIN_CONTROL 0

#ifndef DICE_MODULE_PRIO
    #define DICE_MODULE_PRIO 9999
#else
DICE_HIDE bool
V_JOIN(V_JOIN(ps_dispatch, DICE_MODULE_PRIO), on_)(void)
{
    return true;
}
#endif


#define DICE_MODULE_INIT(CODE)                                                 \
    static bool _module_init()                                                 \
    {                                                                          \
        static bool _done = false;                                             \
        if (!_done) {                                                          \
            _done = true;                                                      \
            do {                                                               \
                CODE                                                           \
            } while (0);                                                       \
            return true;                                                       \
        }                                                                      \
        return false;                                                          \
    }                                                                          \
    static DICE_CTOR void _module_ctr()                                        \
    {                                                                          \
        if (_module_init())                                                    \
            log_printf("[%4d] INIT: %s\n", DICE_MODULE_PRIO, __FILE__);        \
    }                                                                          \
    PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_INIT, {                             \
        if (_module_init())                                                    \
            log_printf("[%4d] INIT! %s\n", DICE_MODULE_PRIO, __FILE__);        \
    })


#define DICE_MODULE_FINI(CODE)                                                 \
    static DICE_DTOR void _module_fini()                                       \
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
#ifdef DICE_BUILTIN
    #define PS_SUBSCRIBE(CHAIN, TYPE, CALLBACK)                                \
        PS_CALLBACK_DECL(CHAIN, TYPE, CALLBACK)                                \
        DICE_HIDE enum ps_cb_err PS_CBNAME(CHAIN, TYPE, DICE_MODULE_PRIO)(     \
            const chain_id chain, const type_id type, void *event,             \
            metadata_t *md)                                                    \
        {                                                                      \
            return PS_CALLBACK(CHAIN, TYPE)(chain, type, event, md);           \
        }                                                                      \
        static void DICE_CTOR _ps_subscribe_##CHAIN##_##TYPE(void)             \
        {                                                                      \
            if (ps_subscribe(CHAIN, TYPE, PS_CALLBACK(CHAIN, TYPE),            \
                             DICE_MODULE_PRIO) != 0)                           \
                log_fatalf("could not subscribe to %s:%s\n", #CHAIN, #TYPE);   \
        }
#else
    #define PS_SUBSCRIBE(CHAIN, TYPE, CALLBACK)                                \
        PS_CALLBACK_DECL(CHAIN, TYPE, CALLBACK)                                \
        static void DICE_CTOR _ps_subscribe_##CHAIN##_##TYPE(void)             \
        {                                                                      \
            if (ps_subscribe(CHAIN, TYPE, PS_CALLBACK(CHAIN, TYPE),            \
                             DICE_MODULE_PRIO) != 0)                           \
                log_fatalf("could not subscribe to %s:%s\n", #CHAIN, #TYPE);   \
        }
#endif

#define PS_CBNAME(X, Y, Z)                                                     \
    V_JOIN(V_JOIN(ps_callback, V_JOIN(X, V_JOIN(Y, Z))), )

#define PS_CALLBACK(CHAIN, TYPE)                                               \
    V_JOIN(V_JOIN(V_JOIN(ps_callback, CHAIN), TYPE), DICE_MODULE_PRIO)

#define PS_CALLBACK_DECL(CHAIN, TYPE, CALLBACK)                                \
    static inline enum ps_cb_err PS_CALLBACK(CHAIN, TYPE)(                     \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        /* Parameters are marked as unused to silence warnings. */             \
        /* Nevertheless, the callback can use parameters without issues. */    \
        (void)chain;                                                           \
        (void)type;                                                            \
        (void)event;                                                           \
        (void)md;                                                              \
                                                                               \
        CALLBACK;                                                              \
                                                                               \
        /* By default, callbacks return OK to continue chain publishing. */    \
        return PS_CB_OK;                                                       \
    }

#endif /* DICE_MODULE_H */
