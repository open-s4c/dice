/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef ORDER_DEFS_H
#define ORDER_DEFS_H

#include <dice/log.h>
#include <dice/pubsub.h>
#define CHAIN 1
#define EVENT 1

struct event {
    int position;
    int count;
};

void publish(struct event *ev);

#define ensure(COND)                                                           \
    {                                                                          \
        if (!(COND)) {                                                         \
            log_fatalf("error: %s\n", #COND);                                  \
        }                                                                      \
    }

#endif /* ORDER_DEFS_H */
