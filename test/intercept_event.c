/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
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

vatomic32_t stop;
vatomic32_t start;
vatomic64_t count;

#define EXPECTED 128

bool received = 0;
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    struct ma_awrite_event *ev = (struct ma_awrite_event *)event;
    if (ev->val.u64 != EXPECTED)
        abort();
    received = 1;
})

int
main(void)
{
    struct ma_awrite_event ev = {0};
    ev.val.u64                = EXPECTED;
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_AWRITE, &ev, 0);
    assert(received);
    return 0;
}
