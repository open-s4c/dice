/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include <dice/chains/intercept.h>
#include <dice/events/memaccess.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/now.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <vsync/atomic.h>

int x = 0;
int y = 0;
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    struct ma_awrite_event *ev = EVENT_PAYLOAD(ev);
    x += ev->val.u64;
    y++;
    return PS_STOP_CHAIN;
})

void
intercept(const chain_id chain, const type_id type, void *event,
          struct metadata *md)
{
    PS_PUBLISH(chain, type, event, md);
}

DICE_MODULE_INIT()
DICE_MODULE_FINI({ log_printf("count: %d\tsum: %d\n", y, x); })
