/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/mempool.h>
#include <dice/pubsub.h>

bool ps_initd_(void);
enum ps_err ps_dispatch_(const chain_id, const type_id, void *, metadata_t *);

DICE_HIDE enum ps_err
ps_publish(const chain_id chain, const type_id type, void *event,
           metadata_t *md)
{
    if (unlikely(!ps_initd_()))
        return PS_DROP_EVENT;

    enum ps_err err = ps_dispatch_(chain, type, event, md);

    if (likely(err == PS_STOP_CHAIN))
        return PS_OK;

    if (likely(err == PS_DROP_EVENT))
        return PS_DROP_EVENT;

    return PS_OK;
}
