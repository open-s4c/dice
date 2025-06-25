/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>

#include <dice/module.h>
#include <dice/pubsub.h>

DICE_HIDE bool
ps_initd_(void)
{
    static enum {
        NONE,
        STARTING,
        INITIALIZING,
        READY,
    } _state = NONE;

    switch (_state) {
        case NONE:
            log_printf("State NONE\n");
            // This must be the main thread, at latest the thread creation
            _state = STARTING;
            PS_PUBLISH(CHAIN_CONTROL, EVENT_DICE_INIT, 0, 0);
            _state = READY;
            log_printf("State READY\n");
            return true;
        case STARTING:
            log_printf("State STARTING\n");
            // The immediate next publish (from above) will publish, but no
            // other publish (no recursive publish).
            _state = INITIALIZING;
            return true;
        case INITIALIZING:
            log_printf("State INITIALIZING\n");
            // We are already initializing above, so we have
            return false;
        default:
            return true;
    }
}
