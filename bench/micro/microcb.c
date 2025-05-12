/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include <bingo/intercept.h>
#include <bingo/intercept/memaccess.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/now.h>
#include <bingo/pubsub.h>
#include <bingo/self.h>
#include <vsync/atomic.h>

int x = 0;
int y = 0;
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    memaccess_t *ma = EVENT_PAYLOAD(ma);
    x += ma->argu64;
    y++;
    return PS_CB_STOP;
})

void
intercept(const chain_id chain, const type_id type, void *event, metadata_t *md)
{
    PS_PUBLISH(chain, type, event, md);
}

BINGO_MODULE_INIT()
BINGO_MODULE_FINI({ log_printf("count: %d\tsum: %d\n", y, x); })
