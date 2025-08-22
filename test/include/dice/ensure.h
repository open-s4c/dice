/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_ENSURE_H
#define DICE_ENSURE_H

#ifdef NDEBUG
    #include <dice/log.h>
    #define ensure(COND)                                                       \
        {                                                                      \
            if (!(COND)) {                                                     \
                log_fatal("error: %s", #COND);                                 \
            }                                                                  \
        }
#else
    #include <assert.h>
    #define ensure assert
#endif
#endif /* DICE_ENSURE_H */
