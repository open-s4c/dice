/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef POSITION
    #error Define POSITION when compiling this file
#endif

#define DICE_MODULE_SLOT POSITION
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
