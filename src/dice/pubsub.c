/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef DICE_ROLL_RUNTIME
    #define DICE_MODULE_SLOT 0
#endif
#include "pubsub-internal.h"
#include "tweaks.h"
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>

#ifdef DICE_ROLL_RUNTIME
    #undef ps_publish
    #undef ps_subscribe
    #define ps_publish dice_publish_internal_
#endif

int ps_dispatch_max(void);
bool ps_dispatch_chain_on_(chain_id);

#if !defined(DICE_ROLL_RUNTIME) || DICE_CALLBACKS
    /* Rolled configurations may map sparse public IDs to compact array indexes.
     */
    #ifndef PS_CHAIN_COUNT
        #define PS_CHAIN_COUNT     MAX_CHAINS
        #define PS_CHAIN_INDEX(ID) (ID)
    #endif
    #ifndef PS_TYPE_COUNT
        #define PS_TYPE_COUNT     MAX_TYPES
        #define PS_TYPE_INDEX(ID) (ID)
    #endif

    #if !defined(DICE_ROLL_RUNTIME) || DICE_PLUGINS
struct chain {
    struct type types[PS_TYPE_COUNT];
};

static struct chain chains_[PS_CHAIN_COUNT];

        #ifdef DICE_ROLL_RUNTIME
static struct type *
ps_callback_list_(chain_id chain, type_id type)
{
    size_t c = PS_CHAIN_INDEX(chain), t = PS_TYPE_INDEX(type);
    if (unlikely(c >= PS_CHAIN_COUNT || !t || t >= PS_TYPE_COUNT))
        return NULL;
    return &chains_[c].types[t];
}
        #endif
    #endif
#endif

// -----------------------------------------------------------------------------
// initializer
// -----------------------------------------------------------------------------

DICE_HIDE bool
ps_initd_(void)
{
    static enum {
        NONE,
        START,
        BLOCK,
    } state_ = NONE;

#if !defined(__clang__)
    static bool ready_ = false;
#endif

    if (likely(ready_)) {
        return true;
    }

    switch (state_) {
        case NONE:
            // This must be the main thread, at latest the thread creation.
            state_ = START;
            log_debug("[%4d] INIT: %s ...", DICE_MODULE_SLOT, __FILE__);
#ifndef DICE_ROLL_RUNTIME
            assert(ps_dispatch_max() >= 0);
#endif
            PS_PUBLISH(CHAIN_DICE_CONTROL, EVENT_DICE_INIT, 0, 0);
            ready_ = true;
            PS_PUBLISH(CHAIN_DICE_CONTROL, EVENT_DICE_READY, 0, 0);
            return true;
        case START:
            state_ = BLOCK;
            return true;
        case BLOCK:
            // The publication above is still running, drop nested publications.
            return false;
        default:
            __builtin_unreachable();
            return true;
    }
}

DICE_HIDE void
ps_init_(void)
{
    (void)ps_initd_();
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_DICE_NOP)
PS_ADVERTISE_TYPE(EVENT_DICE_READY)
PS_ADVERTISE_TYPE(EVENT_DICE_INIT)
PS_DISPATCH_SLOT_ON(DICE_MODULE_SLOT)

// -----------------------------------------------------------------------------
// subscribe interface
// -----------------------------------------------------------------------------

#if !defined(DICE_ROLL_RUNTIME) || DICE_CALLBACKS
static int
ps_subscribe_sorted_(struct sub **cur, chain_id chain, type_id type,
                     ps_callback_f cb, int slot, bool any_type)
{
    // any_type is set if this subscription was from ANY_EVENT
    struct sub *sub;
    struct sub *next = NULL;

    if (*cur == NULL || slot < (*cur)->slot) {
        next = *cur;
        goto insert;
    } else if (slot == (*cur)->slot && !any_type) {
        next = *cur;
        goto insert;
    } else {
        return ps_subscribe_sorted_(&(*cur)->next, chain, type, cb, slot,
                                    any_type);
    }

insert:
    // create and add
    sub = (struct sub *)mempool_alloc(sizeof(struct sub));
    if (!sub)
        return PS_ERROR;
    *sub = (struct sub){.chain = chain,
                        .type  = type,
                        .cb    = cb,
                        .slot  = slot,
                        .next  = next};
    *cur = sub;
    return PS_OK;
}

    #ifdef DICE_ROLL_RUNTIME
DICE_HIDE int
ps_subscribe_list_(struct type *ev, chain_id chain, type_id type,
                   ps_callback_f cb, int slot, bool any_type)
{
    if (!ev)
        return PS_INVALID;

    // register subscription
    int err = ps_subscribe_sorted_(&ev->head, chain, type, cb, slot, any_type);
    if (err == PS_OK)
        ev->count++;
    return err;
}
    #endif

    #if !defined(DICE_ROLL_RUNTIME) || DICE_PLUGINS
static int
ps_subscribe_type_(chain_id chain, type_id type, ps_callback_f cb, int slot,
                   bool any_type)
{
        #ifdef DICE_ROLL_RUNTIME
    return ps_subscribe_list_(ps_callback_list_(chain, type), chain, type, cb,
                              slot, any_type);
        #else
    struct type *ev = &chains_[chain].types[type];
    int err = ps_subscribe_sorted_(&ev->head, chain, type, cb, slot, any_type);
    if (err == PS_OK)
        ev->count++;
    return err;
        #endif
}

DICE_HIDE int
ps_subscribe_(chain_id chain, type_id type, ps_callback_f cb, int slot)
{
    if (chain >= MAX_CHAINS || type >= MAX_TYPES || !cb || slot < 0)
        return PS_INVALID;

        #ifndef DICE_ROLL_RUNTIME
    if (ps_dispatch_chain_on_(chain) && slot <= ps_dispatch_max())
        return PS_OK;
        #endif

    log_debug("=== subscribe %s/%s/%d", ps_chain_str(chain), ps_type_str(type),
              slot);

    if (type != ANY_EVENT)
        return ps_subscribe_type_(chain, type, cb, slot, false);

    int err;
        /* Generation supplies the declared event universe for wildcard
         * expansion.
         */
        #ifdef DICE_ROLL_RUNTIME
    for (size_t k = 0; k < ps_event_count_; k++) {
        type_id i = ps_event_ids_[k];
        #else
    for (size_t i = 1; i < MAX_TYPES; i++) {
        #endif
        if ((err = ps_subscribe_type_(chain, i, cb, slot, true)) != 0)
            return err;
    }

    return PS_OK;
}

        #ifndef DICE_ROLL_RUNTIME
DICE_WEAK int
ps_subscribe(chain_id chain, type_id type, ps_callback_f cb, int slot)
{
    return ps_subscribe_(chain, type, cb, slot);
}
        #endif
    #endif /* legacy / trailing callbacks */

// -----------------------------------------------------------------------------
// publish interface
// -----------------------------------------------------------------------------

    #ifdef DICE_ROLL_RUNTIME
DICE_HIDE enum ps_err
ps_callback_invoke_(struct sub *cur, const chain_id chain, const type_id type,
                    void *event, struct metadata *md)
{
    while (cur) {
        // now we call the callback and abort the chain if the subscriber
        // "censors" the type by returning PS_STOP_CHAIN.
        enum ps_err err = cur->cb(chain, type, event, md);
        if (err == PS_STOP_CHAIN)
            return err;
        cur = cur->next;
    }
    return PS_OK;
}
    #endif

    #if !defined(DICE_ROLL_RUNTIME) || DICE_PLUGINS
static enum ps_err
ps_callback_(const chain_id chain, const type_id type, void *event,
             struct metadata *md)
{
        #ifdef DICE_ROLL_RUNTIME
    struct type *ev = ps_callback_list_(chain, type);
    if (unlikely(!ev))
        return PS_INVALID;
    return ps_callback_invoke_(ev->head, chain, type, event, md);
        #else
    if (unlikely(chain >= MAX_CHAINS))
        return PS_INVALID;
    if (unlikely(type == ANY_EVENT || type >= MAX_TYPES))
        return PS_INVALID;

    struct type *ev = &chains_[chain].types[type];
    struct sub *cur = ev->head;
    while (cur) {
        enum ps_err err = cur->cb(chain, type, event, md);
        if (err == PS_STOP_CHAIN)
            break;
        cur = cur->next;
    }
    return PS_OK;
        #endif
}
    #endif
#endif /* callbacks */

#ifndef DICE_ROLL_RUNTIME
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_(const chain_id chain, const type_id type, void *event,
             struct metadata *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}
#endif

#if !defined(DICE_ROLL_RUNTIME) || DICE_PLUGINS
    #ifdef DICE_ROLL_RUNTIME
DICE_HIDE enum ps_err
    #else
DICE_WEAK enum ps_err
    #endif
ps_publish(const chain_id chain, const type_id type, void *event,
           struct metadata *md)
{
    log_debug("=== ps_publish %s/%s", ps_chain_str(chain), ps_type_str(type));
    if (PS_NOT_INITD_())
        return PS_STOP_CHAIN;

    log_debug("=> publishing %s/%s...", ps_chain_str(chain), ps_type_str(type));
    enum ps_err err = ps_dispatch_(chain, type, event, md);

    if (likely(err == PS_STOP_CHAIN))
        goto end;

    err = ps_callback_(chain, type, event, md);
end:
    log_debug("<= published %s/%s", ps_chain_str(chain), ps_type_str(type));
    return err;
}
#endif

#ifdef DICE_ROLL_RUNTIME
    #undef ps_publish
/* Public API adapters for the configured dispatchers. */
    #if DICE_CHECK_ROUTES
static void
dice_route_error_(const char *direction, int slot, chain_id chain, type_id type)
{
    log_fatal("undeclared %s: slot %d, chain %s (%u), event %s (%u)", direction,
              slot, dice_chain_name_(chain), (unsigned)chain,
              dice_event_name_(type), (unsigned)type);
}
    #endif

void
dice_require_abi(const char *configuration)
{
    if (!configuration || strcmp(configuration, DICE_CONFIG_ID))
        log_fatal("plugin was built with a different generated dice.h");
}

int(ps_subscribe)(chain_id chain, type_id type, ps_callback_f callback,
                  int slot)
{
    #if DICE_CHECK_ROUTES
    if (!(DICE_PLUGINS && slot > DICE_LAST_KNOWN_SLOT) &&
        !dice_consumes_allowed_(slot, chain, type))
        dice_route_error_("consumes", slot, chain, type);
    #endif
    if (!dice_chain_valid_(chain) ||
        (type != ANY_EVENT && !dice_event_valid_(type)) || !callback ||
        slot < 0)
        return PS_INVALID;
    #if DICE_PLUGINS
    if (slot <= DICE_LAST_KNOWN_SLOT)
        return dice_subscribe_known_(chain, type, callback, slot);
    #endif
    return ps_subscribe_(chain, type, callback, slot);
}

int
dice_subscribe_abi(const char *configuration, chain_id chain, type_id type,
                   ps_callback_f callback, int slot)
{
    dice_require_abi(configuration);
    return (ps_subscribe)(chain, type, callback, slot);
}

enum ps_err(ps_publish)(chain_id chain, type_id type, void *event,
                        struct metadata *md)
{
    #if DICE_CHECK_ROUTES
    /* Function pointers cannot carry the caller's slot. */
    dice_route_error_("produces (missing slot context)", -1, chain, type);
    #endif
    if (unlikely(type == ANY_EVENT || type >= MAX_TYPES || chain >= MAX_CHAINS))
        return PS_INVALID;
    return dice_publish_internal_(chain, type, event, md);
}

    #if DICE_CHECK_ROUTES
enum ps_err
dice_publish_checked(int slot, chain_id chain, type_id type, void *event,
                     struct metadata *md)
{
    if (!dice_event_valid_(type) || !dice_chain_valid_(chain) ||
        (!(DICE_PLUGINS && slot > DICE_LAST_KNOWN_SLOT) &&
         !dice_produces_allowed_(slot, chain, type)))
        dice_route_error_("produces", slot, chain, type);
    return dice_publish_internal_(chain, type, event, md);
}
    #endif

const char *
ps_chain_str(chain_id chain)
{
    return dice_chain_name_(chain);
}
const char *
ps_type_str(type_id type)
{
    return dice_event_name_(type);
}

chain_id
ps_chain_lookup(const char *name)
{
    for (unsigned i = 0; name && i < MAX_CHAINS; ++i)
        if (dice_chain_valid_(i) && !strcmp(name, dice_chain_name_(i)))
            return i;
    return MAX_CHAINS;
}

type_id
ps_type_lookup(const char *name)
{
    for (unsigned i = 0; name && i < MAX_TYPES; ++i)
        if ((i == ANY_EVENT || dice_event_valid_(i)) &&
            !strcmp(name, dice_event_name_(i)))
            return i;
    return MAX_TYPES;
}

void
ps_register_chain(chain_id id, const char *name)
{
    if (!dice_chain_valid_(id) || strcmp(dice_chain_name_(id), name))
        log_fatal("chain is not declared by the generated configuration");
}

void
ps_register_type(type_id id, const char *name)
{
    if ((!dice_event_valid_(id) && id != ANY_EVENT) ||
        strcmp(dice_event_name_(id), name))
        log_fatal("event is not declared by the generated configuration");
}

    #ifndef DICE_DISABLE_PUBSUB_CTOR_INIT
static void __attribute__((constructor))
dice_ctor_(void)
{
    ps_init_();
}
    #endif

#endif /* DICE_ROLL_RUNTIME */
