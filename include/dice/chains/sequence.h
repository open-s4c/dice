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
    metadata_t _;
    metadata_t *self;
    chain_id from;
    type_id type;
    thread_id next;
    bool wake;
    bool yield;
};

#endif /* DICE_SEQUENCE_CHAIN_H */
