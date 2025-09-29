/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_TYPES_H
#define DICE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* type_id represents the type of an event. */
typedef uint16_t type_id;

/* MAX_TYPES determines the maximum number of event types. */
#define MAX_TYPES 512

/* ANY_TYPE indicates any event type. */
#define ANY_TYPE 0

/* chain_id identifies a subscriber group ordered by the subscription time. */
typedef uint16_t chain_id;

/* MAX_CHAINS determines the maximum number of chains */
#define MAX_CHAINS 16

/* CHAIN_CONTROL is a chain_id reserved for internal events of the pubsub
 * system, for example, initialization of modules. */
#define CHAIN_CONTROL 0

/* Metadata passed to handlers. It is used to mark events to be dropped by can
 * be extended with type embedding. */
typedef struct metadata {
    bool drop;
} __attribute__((aligned(8))) metadata_t;

#endif /* DICE_TYPES_H */
