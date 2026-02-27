/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define DICE_MODULE_SLOT 0
#include "tweaks.h"
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>

#define MAX_SUBSCRIPTIONS 255

int ps_dispatch_max(void);
bool ps_dispatch_chain_on_(chain_id);

struct sub {
    chain_id chain;
    chain_id type;
    ps_callback_f cb;
    int slot;
    struct sub *next;
};

struct type {
    size_t count;
    struct sub *head;
};

struct chain {
    struct type types[MAX_TYPES];
};

static struct chain chains_[MAX_CHAINS];

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
            PS_PUBLISH(CHAIN_CONTROL, EVENT_DICE_INIT, 0, 0);
            ready_ = true;
            PS_PUBLISH(CHAIN_CONTROL, EVENT_DICE_READY, 0, 0);
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
PS_ADVERTISE_TYPE(EVENT_DICE_READY)
PS_ADVERTISE_TYPE(EVENT_DICE_INIT)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()

// -----------------------------------------------------------------------------
// subscribe interface
// -----------------------------------------------------------------------------

static void
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
        ps_subscribe_sorted_(&(*cur)->next, chain, type, cb, slot, any_type);
        return;
    }

insert:
    // create and add
    sub  = (struct sub *)mempool_alloc(sizeof(struct sub));
    *sub = (struct sub){.chain = chain,
                        .type  = type,
                        .cb    = cb,
                        .slot  = slot,
                        .next  = next};
    *cur = sub;
}

static int
ps_subscribe_type_(chain_id chain, type_id type, ps_callback_f cb, int slot,
                   bool any_type)
{
    if (type > MAX_TYPES)
        return PS_INVALID;

    struct type *ev = &chains_[chain].types[type];

    // increment the idx of next subscription
    ev->count++;

    // register subscription
    ps_subscribe_sorted_(&ev->head, chain, type, cb, slot, any_type);

    return PS_OK;
}

static int
ps_subscribe_(chain_id chain, type_id type, ps_callback_f cb, int slot)
{
    ps_init_(); // ensure initialized

    log_print(ULTRA, "Subscribe %s/%s/%d", ps_chain_str(chain),
              ps_type_str(type), slot);
    if (ps_dispatch_chain_on_(chain) && slot <= ps_dispatch_max()) {
        log_print(ULTRA, "Ignore subscription %s/%s/%d", ps_chain_str(chain),
                  ps_type_str(type), slot);
        return PS_OK;
    }
    if (chain == CHAIN_CONTROL)
        return PS_OK;
    if (chain > MAX_CHAINS)
        return PS_INVALID;
    if (type != ANY_EVENT)
        return ps_subscribe_type_(chain, type, cb, slot, false);

    int err;
    for (size_t i = 1; i < MAX_TYPES; i++)
        if ((err = ps_subscribe_type_(chain, i, cb, slot, true)) != 0)
            return err;

    return PS_OK;
}

DICE_WEAK int
ps_subscribe(chain_id chain, type_id type, ps_callback_f cb, int slot)
{
    return ps_subscribe_(chain, type, cb, slot);
}

// -----------------------------------------------------------------------------
// publish interface
// -----------------------------------------------------------------------------

static enum ps_err
ps_callback_(const chain_id chain, const type_id type, void *event,
             struct metadata *md)
{
    if (unlikely(chain >= MAX_CHAINS))
        return PS_INVALID;
    if (unlikely(type == ANY_EVENT || type >= MAX_TYPES))
        return PS_INVALID;

    struct type *ev = &chains_[chain].types[type];
    struct sub *cur = ev->head;
    while (cur) {
        // now we call the callback and abort the chain if the subscriber
        // "censors" the type by returning PS_STOP_CHAIN.
        enum ps_err err = cur->cb(chain, type, event, md);
        if (err == PS_STOP_CHAIN)
            break;
        if (err == PS_DROP_EVENT)
            return PS_DROP_EVENT;
        cur = cur->next;
    }
    return PS_OK;
}

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

DICE_WEAK enum ps_err
ps_publish(const chain_id chain, const type_id type, void *event,
           struct metadata *md)
{
    if (PS_NOT_INITD_())
        return PS_DROP_EVENT;

    log_print(ULTRA, "Publish %s/%s", ps_chain_str(chain), ps_type_str(type));
    enum ps_err err = ps_dispatch_(chain, type, event, md);

    if (likely(err == PS_STOP_CHAIN))
        return PS_OK;

    if (likely(err == PS_DROP_EVENT))
        return PS_DROP_EVENT;

    return ps_callback_(chain, type, event, md);
}
