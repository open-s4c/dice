/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/mempool.h>
#include <dice/pubsub.h>

// -----------------------------------------------------------------------------
// pubsub interface
// -----------------------------------------------------------------------------

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

DICE_HIDE int
ps_subscribe(chain_id chain, type_id type, ps_callback_f cb, int prio)
{
    (void)chain;
    (void)type;
    (void)cb;
    (void)prio;
    return PS_OK;
}

// -----------------------------------------------------------------------------
// mempool interface
// -----------------------------------------------------------------------------

void *mempool_alloc_(size_t size);
DICE_HIDE void *
mempool_alloc(size_t size)
{
    return mempool_alloc_(size);
}

void *mempool_realloc_(void *ptr, size_t size);
DICE_HIDE void *
mempool_realloc(void *ptr, size_t size)
{
    return mempool_realloc_(ptr, size);
}

void mempool_free_(void *ptr);
DICE_HIDE void
mempool_free(void *ptr)
{
    return mempool_free_(ptr);
}
