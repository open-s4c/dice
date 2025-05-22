/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>

#define BINGO_XTOR_PRIO 199
#include <bingo/module.h>
#include <bingo/pubsub.h>

static bool _initd;

BINGO_HIDE int
ps_subscribe(chain_id chain, type_id type, ps_cb_f cb)
{
    (void)chain;
    (void)type;
    (void)cb;
    return 0;
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

BINGO_HIDE enum ps_err
ps_publish(const chain_id chain, const type_id type, void *event,
           metadata_t *md)
{
    struct ps_dispatched ret = ps_dispatch_(chain, type, event, md);
    if (likely(ret.err == PS_CB_STOP))
        return PS_OK;

    if (likely(ret.err == PS_CB_DROP))
        return PS_DROP;

    return PS_OK;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------

BINGO_MODULE_INIT({ _initd = true; })
