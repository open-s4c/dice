/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef DICE_LOG_H
#define DICE_LOG_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define LOG_MSG_MAX 1024

#define log_printf(fmt, ...)                                                   \
    do {                                                                       \
        char msg[LOG_MSG_MAX];                                                 \
        int n = snprintf(msg, sizeof(msg), fmt, ##__VA_ARGS__);                \
        if (write(STDOUT_FILENO, msg, n) == -1) {                              \
            perror("write stdout");                                            \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)

#define log_debugf log_printf
#define log_fatalf(fmt, ...)                                                   \
    {                                                                          \
        log_printf(fmt, ##__VA_ARGS__);                                        \
        abort();                                                               \
    }

#endif /* DICE_LOG_H */
