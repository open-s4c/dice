/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef POSITION
    #error Define POSITION when compiling this file
#endif

#define DICE_XTOR_PRIO POSITION
#include "defs.h"
#include <dice/module.h>
#include <dice/pubsub.h>

DICE_MODULE_INIT()

PS_SUBSCRIBE(CHAIN, EVENT, {
    struct event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->position < POSITION);
    ev->position = POSITION;
    ev->count++;
})
