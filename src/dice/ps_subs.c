/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <dice/module.h>
#include <dice/pubsub.h>

/* -----------------------------------------------------------------------------
 * subscriptions
 * -------------------------------------------------------------------------- */

DICE_WEAK enum ps_cb_err
ps_callback_0_99_197_(const chain_id chain, const type_id type, void *event,
                      metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_CB_OK;
}

DICE_WEAK enum ps_cb_err
ps_callback_0_99_198_(const chain_id chain, const type_id type, void *event,
                      metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_CB_OK;
}

DICE_WEAK enum ps_cb_err
ps_callback_0_99_199_(const chain_id chain, const type_id type, void *event,
                      metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_CB_OK;
}

DICE_WEAK struct ps_dispatched
ps_dispatch_(const chain_id chain, const type_id type, void *event,
             metadata_t *md)
{
    struct ps_dispatched s = {.err = PS_CB_OFF};
    if (chain != CHAIN_CONTROL)
        return (struct ps_dispatched){.err = PS_CB_OFF};

    s.err = ps_callback_0_99_197_(chain, type, event, md);
    if (s.err != PS_CB_OK)
        return s;
    s.err = ps_callback_0_99_198_(chain, type, event, md);
    if (s.err != PS_CB_OK)
        return s;
    s.err = ps_callback_0_99_199_(chain, type, event, md);
    if (s.err != PS_CB_OK)
        return s;

    return (struct ps_dispatched){.err = PS_CB_OK};
}
