/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <stdio.h>

#define BINGO_XTOR_PRIO 199
#include <bingo/module.h>
#include <bingo/pubsub.h>

#define MAX_SUBSCRIPTIONS 255

struct sub {
    chain_id chain;
    chain_id type;
    ps_cb_f cb;
};

struct type {
    size_t count;
    struct sub subs[MAX_SUBSCRIPTIONS + 1];
};

struct chain {
    struct type types[MAX_TYPES];
};

static bool _initd;
static struct chain _chains[MAX_CHAINS];

// -----------------------------------------------------------------------------
// subscribe interface
// -----------------------------------------------------------------------------

static int
_ps_subscribe_type(chain_id chain, type_id type, ps_cb_f cb)
{
    if (type > MAX_TYPES)
        return PS_INVALID;

    struct type *ev = &_chains[chain].types[type];
    size_t subs_idx = ev->count;
    if (subs_idx >= MAX_SUBSCRIPTIONS)
        return PS_INVALID;

    // increment the idx of next subscription
    ev->count++;

    // register subscription
    ev->subs[subs_idx] = (struct sub){.chain = chain, .type = type, .cb = cb};

    return 0;
}

int
ps_subscribe(chain_id chain, type_id type, ps_cb_f cb)
{
    if (chain == 0 || chain > MAX_CHAINS)
        return PS_INVALID;
    if (type != ANY_TYPE)
        return _ps_subscribe_type(chain, type, cb);

    int err;
    for (size_t i = 1; i < MAX_TYPES; i++)
        if ((err = _ps_subscribe_type(chain, i, cb)) != 0)
            return err;

    return 0;
}

// -----------------------------------------------------------------------------
// publish interface
// -----------------------------------------------------------------------------

static enum ps_err
_ps_publish(const chain_id chain, const type_id type, void *event,
            metadata_t *md, size_t start)
{
    if (unlikely(chain >= MAX_CHAINS))
        return PS_INVALID;
    if (unlikely(type == ANY_TYPE || type >= MAX_TYPES))
        return PS_INVALID;

    struct type *ev = &_chains[chain].types[type];
    for (size_t idx = start; idx < ev->count; idx++) {
        struct sub *subs = &ev->subs[idx];
        // now we call the callback and abort the chain if the subscriber
        // "censors" the type by returning PS_CB_STOP.
        enum ps_cb_err err = subs->cb(chain, type, event, md);
        if (err == PS_CB_STOP)
            break;
        if (err == PS_CB_DROP)
            return PS_DROP;
    }
    return PS_OK;
}

BINGO_WEAK BINGO_HIDE struct ps_dispatched
ps_dispatch_(const chain_id chain, const type_id type, void *event,
             metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return (struct ps_dispatched){.err = PS_CB_OFF};
}

enum ps_err
ps_publish(const chain_id chain, const type_id type, void *event,
           metadata_t *md)
{
    if (unlikely(!_initd))
        return PS_DROP;

    struct ps_dispatched ret = ps_dispatch_(chain, type, event, md);
    if (likely(ret.err == PS_CB_STOP))
        return PS_OK;

    if (likely(ret.err == PS_CB_DROP))
        return PS_DROP;

    return _ps_publish(chain, type, event, md, ret.count);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

BINGO_MODULE_INIT({ _initd = true; })
