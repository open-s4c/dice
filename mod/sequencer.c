/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <bingo/intercept/memaccess.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/self.h>
#include <bingo/switcher.h>

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_TYPE, {
    switch (type) {
        case EVENT_THREAD_FINI:
            switcher_wake(ANY_THREAD, 0);
            break;
        case EVENT_THREAD_INIT:
            /* threads call this only ONCE (except the main thread). */
            switcher_yield(self_id(md), true);
            break;
        default:
            break;
    }
    return false;
})

PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_TYPE, {
    switcher_wake(ANY_THREAD, 0);
    return false;
})

PS_SUBSCRIBE(CAPTURE_AFTER, ANY_TYPE, {
    switcher_yield(self_id(md), true);
    return false;
})

BINGO_MODULE_INIT()
