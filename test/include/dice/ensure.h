/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_ENSURE_H
#define DICE_ENSURE_H

#include <dice/log.h>
#define ensure(COND)                                                           \
    {                                                                          \
        if (!(COND)) {                                                         \
            log_abort("cannot ensure: %s", #COND);                             \
        }                                                                      \
    }

#endif /* DICE_ENSURE_H */
