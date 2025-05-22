/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include <dice/intercept.h>
#include <dice/intercept/memaccess.h>
#include <dice/log.h>
#include <dice/now.h>
#include <dice/self.h>
#include <vsync/atomic.h>

vatomic32_t stop;
vatomic32_t start;
vatomic64_t count;

#define EXPECTED 128

bool received = 0;
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_MA_AWRITE, {
    memaccess_t *ma = (memaccess_t *)event;
    if (ma->argu64 != EXPECTED)
        abort();
    received = 1;
})

int
main(void)
{
    memaccess_t ma = {.argu64 = EXPECTED};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_MA_AWRITE, &ma, 0);

    assert(received);
    return 0;
}
