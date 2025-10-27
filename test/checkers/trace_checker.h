/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_TRACE_CHECKER_H
#define DICE_TRACE_CHECKER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <dice/pubsub.h>

#define CHECK_FUNC __attribute__((no_sanitize("thread")))

struct expected_event {
    chain_id chain;
    type_id type;
    unsigned atleast;
    unsigned atmost;
    bool wild;
    bool set;
};

#define EXPECTED_EVENT(CHAIN, TYPE)                                            \
    (struct expected_event)                                                    \
    {                                                                          \
        .chain = CHAIN, .type = TYPE, .atleast = 1, .atmost = 1,               \
        .wild = false, .set = true,                                            \
    }
#define EXPECTED_SUFFIX(CHAIN, TYPE)                                           \
    (struct expected_event)                                                    \
    {                                                                          \
        .chain = CHAIN, .type = TYPE, .atleast = 1, .atmost = 1, .wild = true, \
        .set = true,                                                           \
    }
#define EXPECTED_SOME(CHAIN, TYPE, ATLEAST, ATMOST)                            \
    (struct expected_event)                                                    \
    {                                                                          \
        .chain = CHAIN, .type = TYPE, .atleast = ATLEAST, .atmost = ATMOST,    \
        .wild = false, .set = true,                                            \
    }
#define EXPECTED_ANY_SUFFIX EXPECTED_SUFFIX(0, 0)

#define EXPECTED_END                                                           \
    (struct expected_event)                                                    \
    {                                                                          \
        .chain = 0, .type = 0, .atleast = 0, .atmost = 0, .wild = false,       \
        .set = false,                                                          \
    }


void register_expected_trace(thread_id id, struct expected_event *trace);

#endif // DICE_TRACE_CHECKER_H
