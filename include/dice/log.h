/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_LOG_H
#define DICE_LOG_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef LOG_UNLOCKED
    #define LOG_LOCK_ACQUIRE
    #define LOG_LOCK_RELEASE
#else
    #include <dice/compiler.h>
    #include <vsync/spinlock/caslock.h>
    #define LOG_LOCK_ACQUIRE caslock_acquire(&log_lock);
    #define LOG_LOCK_RELEASE caslock_release(&log_lock);
DICE_WEAK caslock_t log_lock;
#endif

#ifndef LOG_FILENO
    #define LOG_FILENO STDOUT_FILENO
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

#define LOG_PASTE(LEVEL)  LOG_LEVEL##_##LEVEL
#define LOG_EXPAND(LEVEL) LOG_PASTE(LEVEL)
#define LOG_LEVEL_        LOG_EXPAND(LOG_LEVEL)

#define LOG_MAX_LEN 1024

static void __attribute__((noinline, unused))
dice_log_vprintf_(const char *fmt, va_list ap)
{
    char msg[LOG_MAX_LEN];
    int n = vsnprintf(msg, sizeof(msg), fmt, ap);
    if (n < 0) {
        const char err[] = "vsnprintf failed";
        ssize_t e = write(STDERR_FILENO, err, sizeof(err) - 1);
        (void)e;
        exit(EXIT_FAILURE);
    }

    size_t len = (size_t)n;
    if (len >= sizeof(msg))
        len = sizeof(msg) - 1;

    ssize_t w = write(LOG_FILENO, msg, len);
    if (w < 0 || (size_t)w != len) {
        const char err[] = "write failed";
        ssize_t e = write(STDERR_FILENO, err, sizeof(err) - 1);
        (void)e;
        exit(EXIT_FAILURE);
    }
}

static void __attribute__((noinline, unused, format(printf, 1, 2)))
dice_log_printf_(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    dice_log_vprintf_(fmt, ap);
    va_end(ap);
}

#define log_printf(...) dice_log_printf_(__VA_ARGS__)

#define log_warn(fmt, ...)                                                     \
    do {                                                                       \
        LOG_LOCK_ACQUIRE;                                                      \
        log_printf(LOG_PREFIX);                                                \
        log_printf(fmt, ##__VA_ARGS__);                                        \
        log_printf(LOG_SUFFIX);                                                \
        LOG_LOCK_RELEASE;                                                      \
    } while (0)

#if LOG_LEVEL_ >= LOG_LEVEL_DEBUG
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

#if LOG_LEVEL_ >= LOG_LEVEL_INFO
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

/* log_fatal always prints the message and exits with EXIT_FAILURE.
 *
 * It indicates a fatal error was detected, but it is a possibly expected error
 * (in contrast to abort below).
 */
#define log_fatal(fmt, ...)                                                    \
    do {                                                                       \
        LOG_LOCK_ACQUIRE;                                                      \
        log_printf(LOG_PREFIX);                                                \
        log_printf(fmt, ##__VA_ARGS__);                                        \
        log_printf(LOG_SUFFIX);                                                \
        LOG_LOCK_RELEASE;                                                      \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

/* log_abort always prints the message and aborts the program.
 *
 * It indicates an unexpected error was detected, ie, an internal bug in Dice or
 * one of its modules.
 */
#define log_abort(fmt, ...)                                                    \
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
