/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>

static void
event_log_(struct metadata *md, chain_id chain, type_id type)
{
    log_printf("[tid: %" PRIx64 "] %s\t%s\n", self_id(md), ps_chain_str(chain),
               ps_type_str(type));
}

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_EVENT, { event_log_(md, chain, type); })
PS_SUBSCRIBE(CAPTURE_AFTER, ANY_EVENT, { event_log_(md, chain, type); })
PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_EVENT, { event_log_(md, chain, type); })

DICE_MODULE_INIT()
