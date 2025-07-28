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

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 2

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifndef LOG_PREFIX
    #define LOG_PREFIX "dice: "
#endif

#ifndef LOG_SUFFIX
    #define LOG_SUFFIX "\n"
#endif

#define LOG_MAX_LEN 1024
#define log_printf(fmt, ...)                                                   \
    do {                                                                       \
        char msg[LOG_MAX_LEN];                                                 \
        int n = snprintf(msg, LOG_MAX_LEN, fmt, ##__VA_ARGS__);                \
        if (write(STDOUT_FILENO, msg, n) == -1) {                              \
            perror("write stdout");                                            \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define log_debug(fmt, ...)                                                \
        do {                                                                   \
            log_printf(LOG_PREFIX);                                            \
            log_printf(fmt, ##__VA_ARGS__);                                    \
            log_printf(LOG_SUFFIX);                                            \
        } while (0)
#else
    #define log_debug log_none
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define log_info(fmt, ...)                                                 \
        do {                                                                   \
            log_printf(LOG_PREFIX);                                            \
            log_printf(fmt, ##__VA_ARGS__);                                    \
            log_printf(LOG_SUFFIX);                                            \
        } while (0)
#else
    #define log_info log_none
#endif

#define log_fatal(fmt, ...)                                                    \
    do {                                                                       \
        log_printf(LOG_PREFIX);                                                \
        log_printf(fmt, ##__VA_ARGS__);                                        \
        log_printf(LOG_SUFFIX);                                                \
        abort();                                                               \
    } while (0)

#define log_none(...)                                                          \
    do {                                                                       \
    } while (0)

#endif /* DICE_LOG_H */
