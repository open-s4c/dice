/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include "defs.h"
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

DICE_MODULE_INIT()

INTERPOSE(void, publish, struct event *ev)
{
    (void)ev;
    PS_PUBLISH(CHAIN, EVENT, ev, 0);
}
