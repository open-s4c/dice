/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_THREAD_ID_H
#define DICE_THREAD_ID_H

#include <stdint.h>

typedef uint64_t thread_id;
#define NO_THREAD   ((thread_id)0)
#define ANY_THREAD  (~NO_THREAD)
#define MAIN_THREAD 1

#define EVENT_THREAD_INIT 1
#define EVENT_THREAD_FINI 2

#endif /* DICE_THREAD_ID_H */
