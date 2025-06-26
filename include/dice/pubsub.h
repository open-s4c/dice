/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file pubsub.h
 * @brief Interruptable chain-based pubsub
 *
 * This component provides a chain-based pubsub. When an event is published to a
 * chain (aka topic), handlers are called back in the order of subscription.
 * Each handler has the option of interruping the chain by returning false.
 *
 * Events are have a type (`type_id`) and have a payload `payload`. A chain is
 * identified by a `chain_id`.
 *
 * ## Naming
 *
 * For a lack of better name, this component is called `pubsub`. It combines
 * aspects of several GoF design patterns such as pubsub, observer, and
 * chain-of-responsibility pattern. However, it violates some requirements of
 * each aspect such that no name seem to perfectly match it.
 *
 */
#ifndef DICE_PUBSUB_H
#define DICE_PUBSUB_H
#include <stdbool.h>
#include <stdint.h>

#include <dice/compiler.h>
#include <dice/log.h>
#include <vsync/atomic.h>
#include <vsync/atomic/internal/macros.h>

/* type_id represents the type of an event. */
typedef uint16_t type_id;

/* MAX_TYPES determines the maximum number of event types. */
#define MAX_TYPES 512

/* ANY_TYPE indicates any event type. */
#define ANY_TYPE 0

/* chain_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t chain_id;

/* MAX_CHAINS determines the maximum number of chains */
#define MAX_CHAINS 16

/* Metadata passed to callbacks. It is used to mark events to be dropped by can
 * be extended with type embedding. */
typedef struct metadata {
    bool drop;
} __attribute__((aligned(8))) metadata_t;

/* Return values of ps_publish. */
enum ps_err {
    PS_OK      = 0,
    PS_DROP    = -2,
    PS_INVALID = -4,
    PS_ERROR   = -5,
};

/* Return values of callbacks. */
enum ps_cb_err {
    PS_CB_OK   = 1,
    PS_CB_OFF  = 0,
    PS_CB_STOP = -1,
    PS_CB_DROP = -2,
};

/* Return values of ps_dispatch. */
struct ps_dispatched {
    enum ps_cb_err err;
    uint8_t count;
};

/* ps_callback_f is the subscriber interface.
 *
 * Callbacks can return the following codes:
 * - PS_OK: event handled successfully
 * - PS_STOP: event handled successfully, but chain should be interrupted
 * - PS_OFF: callback disabled
 */
typedef enum ps_cb_err (*ps_cb_f)(const chain_id, const type_id, void *event,
                                  metadata_t *md);

/* ps_publish publishes (ie, dispatches) an event to a chain.
 *
 * The chain is identified by a chain and a event type. The type of the
 * event is given by `event`. Arguments `arg` and `ret` are input and output
 * arguments, respectively. They can be set to NULL. It is thread of the
 * subscribing handler to test for NULL and to cast `arg` and `ret` to
 * correct types based on `event`.
 *
 * Returns one of the PS_ error codes above.
 */
enum ps_err ps_publish(const chain_id chain, const type_id type, void *event,
                       metadata_t *md);

#define PS_PUBLISH(chain, type, event, md)                                     \
    do {                                                                       \
        metadata_t __md = {0};                                                 \
        metadata_t *_md = (md) != NULL ? (metadata_t *)(md) : &__md;           \
        if (_md->drop)                                                         \
            break;                                                             \
        enum ps_err err = ps_publish(chain, type, event, md);                  \
        if (err == PS_DROP)                                                    \
            _md->drop = true;                                                  \
        if (err != PS_OK && err != PS_DROP)                                    \
            log_fatalf("could not publish\n");                                 \
    } while (0)

/* ps_subscribe subscribes a callback in a chain for an event.
 *
 * `prio` determines the relative order in which the callbacks are called with
 * published events.
 *
 * Note: ps_subscribe should only be called during initialization of the
 * system.
 *
 * Returns 0 if success, otherwise non-zero.
 */
int ps_subscribe(chain_id chain, type_id type, ps_cb_f cb, int prio);

#define PS_CBNAME(X, Y, Z)                                                     \
    V_JOIN(V_JOIN(ps_callback, V_JOIN(X, V_JOIN(Y, Z))), )

/* PS_SUBSCRIBE macro creates a callback handler and subscribes to a
 * chain.
 *
 * On load time, a constructor function registers the handler to the
 * chain. The order in which modules are loaded must be considered when
 * planning for the relation between handlers. The order is either given
 * by linking order (if compilation units are linked together) or by the
 * order of shared libraries in LD_PRELOAD.
 */
#define PS_SUBSCRIBE(CHAIN, TYPE, CALLBACK)                                    \
    static inline enum ps_cb_err _ps_callback_##CHAIN##_##TYPE(                \
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
    }                                                                          \
    DICE_HIDE enum ps_cb_err PS_CBNAME(CHAIN, TYPE, DICE_XTOR_PRIO)(           \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        return _ps_callback_##CHAIN##_##TYPE(chain, type, event, md);          \
    }                                                                          \
    static enum ps_cb_err V_JOIN(V_JOIN(_ps_callback, CHAIN), TYPE)(           \
        const chain_id chain, const type_id type, void *event, metadata_t *md) \
    {                                                                          \
        return _ps_callback_##CHAIN##_##TYPE(chain, type, event, md);          \
    }                                                                          \
    static void DICE_CTOR _ps_subscribe_##CHAIN##_##TYPE(void)                 \
    {                                                                          \
        if (ps_subscribe(CHAIN, TYPE,                                          \
                         V_JOIN(V_JOIN(_ps_callback, CHAIN), TYPE),            \
                         (DICE_XTOR_PRIO - 1)) != 0)                           \
            log_fatalf("could not subscribe to %s:%s\n", #CHAIN, #TYPE);       \
    }

/* EVENT_PAYLOAD casts the event argument `event` to type of the given
 * variable.
 *
 * This macro is intended to be used with PS_SUBSCRIBE. The user must
 * know the type of the argument and then the following pattern can be used:
 *
 *     PS_SUBSCRIBE(SOME_HOOK, SOME_EVENT, {
 *         some_known_type *ev = EVENT_PAYLOAD(ev);
 *         ...
 *         })
 */
#define EVENT_PAYLOAD(var) (__typeof(var))event

#define INTERPOSE_PC                                                           \
    (__builtin_extract_return_addr(__builtin_return_address(0)))

#endif /* DICE_PUBSUB_H */
