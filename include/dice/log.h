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

#ifndef LOG_LOCKED
    #define LOG_LOCK_ACQUIRE
    #define LOG_LOCK_RELEASE
#else
    #include <dice/compiler.h>
    #include <vsync/spinlock/caslock.h>
    #define LOG_LOCK_ACQUIRE caslock_acquire(&log_lock);
    #define LOG_LOCK_RELEASE caslock_release(&log_lock);
DICE_WEAK caslock_t log_lock;
#endif

#define LOG_LEVEL_FATAL 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_DEBUG 2

#ifndef LOG_LEVEL
    #define LOG_LEVEL INFO
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

#define LOG_PASTE(LEVEL)  LOG_LEVEL##_##LEVEL
#define LOG_EXPAND(LEVEL) LOG_PASTE(LEVEL)

#if LOG_EXPAND(LOG_LEVEL) >= LOG_LEVEL_DEBUG
    #define log_debug(fmt, ...)                                                \
        do {                                                                   \
            LOG_LOCK_ACQUIRE;                                                  \
            log_printf(LOG_PREFIX);                                            \
            log_printf(fmt, ##__VA_ARGS__);                                    \
            log_printf(LOG_SUFFIX);                                            \
            LOG_LOCK_RELEASE;                                                  \
        } while (0)
#else
    #define log_debug log_none
#endif

#if LOG_EXPAND(LOG_LEVEL) >= LOG_LEVEL_INFO
    #define log_info(fmt, ...)                                                 \
        do {                                                                   \
            LOG_LOCK_ACQUIRE;                                                  \
            log_printf(LOG_PREFIX);                                            \
            log_printf(fmt, ##__VA_ARGS__);                                    \
            log_printf(LOG_SUFFIX);                                            \
            LOG_LOCK_RELEASE;                                                  \
        } while (0)
#else
    #define log_info log_none
#endif

#define log_fatal(fmt, ...)                                                    \
    do {                                                                       \
        LOG_LOCK_ACQUIRE;                                                      \
        log_printf(LOG_PREFIX);                                                \
        log_printf(fmt, ##__VA_ARGS__);                                        \
        log_printf(LOG_SUFFIX);                                                \
        LOG_LOCK_RELEASE;                                                      \
        abort();                                                               \
    } while (0)

#define log_none(...)                                                          \
    do {                                                                       \
    } while (0)

#endif /* DICE_LOG_H */
