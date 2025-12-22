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
            log_abort("%s:%d: cannot ensure: %s", __FILE__, (int)(__LINE__),   \
                      #COND);                                                  \
        }                                                                      \
    }

#endif /* DICE_ENSURE_H */
