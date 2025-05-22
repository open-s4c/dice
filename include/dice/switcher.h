/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_SWITCHER_H
#define DICE_SWITCHER_H

#include <stdbool.h>

#include <dice/now.h>
#include <dice/thread_id.h>

#ifndef SWITCHER_NBUCKETS
    #define SWITCHER_NBUCKETS 128
#endif

int switcher_yield(thread_id id, bool any);
void switcher_wake(thread_id id, nanosec_t slack);
void switcher_abort(void);

#define SWITCHER_CONTINUE 0
#define SWITCHER_CHANGED  1
#define SWITCHER_ABORTED  2

#endif /* DICE_SWITCHER_H */
