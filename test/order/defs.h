/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef ORDER_DEFS_H
#define ORDER_DEFS_H

#include <dice/ensure.h>
#include <dice/log.h>
#include <dice/pubsub.h>
#define CHAIN 1
#define EVENT 1

struct event {
    int position;
    int count;
};

void publish(struct event *ev);

#endif /* ORDER_DEFS_H */
