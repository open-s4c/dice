/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_TYPES_H
#define DICE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* type_id represents the type of an event. Modules declare their own event
 * identifiers in the headers under include/dice/events/. */
typedef uint16_t type_id;

/* MAX_TYPES determines the maximum number of event types. */
#define MAX_TYPES 256

/* ANY_EVENT indicates any event type. */
#define ANY_EVENT 0

/* chain_id identifies a subscriber group ordered by the subscription time.
 * See include/dice/chains/ for the predefined chain identifiers. */
typedef uint16_t chain_id;

/* MAX_CHAINS determines the maximum number of chains */
#define MAX_CHAINS 16

/* CHAIN_CONTROL is a chain_id reserved for internal events of the pubsub
 * system, for example, initialization of modules. */
#define CHAIN_CONTROL 0

/* Metadata passed to handlers. The base definition tracks drop semantics; chain
 * implementations embed additional fields by casting to chain-specific
 * structures. */
typedef struct metadata {
    bool drop;
} __attribute__((aligned(8))) metadata_t;

/* MAX_BUILTIN_SLOTS determines the maximum number of builtin slots. The range
 * 0..MAX_BUILTIN_SLOTS-1 is reserved for builtin modules. */
#define MAX_BUILTIN_SLOTS 16

/* Dice assigns compact thread identifiers that remain stable for the lifetime
 * of a thread.
 *
 * For the set of Threads in the program the following two properties hold:
 *
 * - Validity:
 *     \A t \in Threads:
 *        /\ t.id \in thread_id
 *        /\ t.id \notin { NO_THREAD, ANY_THREAD}
 *
 * - Uniqueness:
 *     \A t1, t2 \in Threads:
 *        ~Same(t1, t2) => t1.id != t2.id
 */
typedef uint64_t thread_id;

/* NO_THREAD used to represent an invalid thread ID */
#define NO_THREAD ((thread_id)0)

/* ANY_THREAD used to represent a valid but unknown thread ID */
#define ANY_THREAD (~NO_THREAD)

/* MAIN_THREAD is always the ID of the first thread in the program */
#define MAIN_THREAD 1

#endif /* DICE_TYPES_H */
