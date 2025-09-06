/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_SEQUENCE_CHAIN_H
#define DICE_SEQUENCE_CHAIN_H
#include <stdbool.h>

#include <dice/pubsub.h>
#include <dice/self.h>

#define SEQUENCE_EVENT  7
#define SEQUENCE_BEFORE 8
#define SEQUENCE_AFTER  9

struct plan {
    metadata_t _;     // subtype plan from metadata
    metadata_t *self; // link to previous metadata
    chain_id chain;   // chain from where the event arrived
    type_id type;     // event type
    thread_id me;     // current thread id (cached self_id(plan.self)
    thread_id next;   // thread id of next thread to be scheduled (if any)
    bool wake;        // wake next thread
    bool yield;       // yield current thread
};

#endif /* DICE_SEQUENCE_CHAIN_H */
