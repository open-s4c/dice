/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef EVENTS_SELF_H
#define EVENTS_SELF_H

#include <stdbool.h>

#define EVENT_SELF_INIT 5
#define EVENT_SELF_FINI 6
#define EVENT_SELF_WAIT 7

struct self_wait_event {
    bool wait;
};

#endif /* EVENTS_SELF_H */
