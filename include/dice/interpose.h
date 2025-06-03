/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 * -----------------------------------------------------------------------------
 * @file interpose.h
 * @brief Interposition helper macros
 *
 * The macro `INTERPOSE` can be used to create interceptors for arbitrary
 * functions from dynamic libraries. For example, the function malloc can be
 * interposed like this:
 *
 *     INTERPOSE(void *, malloc, size_t n)
 *     {
 *         // do something before the real call
 *         void *ptr = REAL(malloc, n);
 *         // do somethine after the real call
 *         return ptr;
 *     }
 *
 * This header used in several modules in `mod/` in combination with the
 * publishing functions provided by `intercept.h`.
 *
 * See `mod/malloc.c` for a detailed example.
 */
#ifndef DICE_INTERPOSE_H
#define DICE_INTERPOSE_H
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

#include <dice/module.h>

#define REAL_NAME(F)         _dice_real_##F
#define REAL_DECL(T, F, ...) static T (*REAL_NAME(F))(__VA_ARGS__);
#define REAL_CALL(F, ...)                                                      \
    ((REAL_NAME(F) == NULL ?                                                   \
          (REAL_NAME(F) = (__typeof(REAL_NAME(F)))dlsym(RTLD_NEXT, #F)) :      \
          0),                                                                  \
     REAL_NAME(F)(__VA_ARGS__))

#if defined(__linux__) || defined(__NetBSD__)
    #include <dlfcn.h>

    #define REAL(F, ...) REAL_CALL(F, ##__VA_ARGS__)

    #define INTERPOSE(T, F, ...)                                               \
        T F(__VA_ARGS__);                                                      \
        static T (*REAL_NAME(F))(__VA_ARGS__);                                 \
        T F(__VA_ARGS__)

#elif defined(__APPLE__)

    #define REAL(F, ...) F(__VA_ARGS__)
    #define FAKE_NAME(F) _dice_fake_##F

    #define INTERPOSE(T, F, ...)                                               \
        T FAKE_NAME(F)(__VA_ARGS__);                                           \
        static struct {                                                        \
            const void *fake;                                                  \
            const void *real;                                                  \
        } _dice_interpose_##F                                                  \
            __attribute__((used, section("__DATA,__interpose"))) = {           \
                (const void *)&FAKE_NAME(F), (const void *)&F};                \
        T FAKE_NAME(F)(__VA_ARGS__)
#else
    #error Unsupported platform
#endif

#if defined(__linux__)
    #define REAL_CALL_VERSION(F, V, ...)                                       \
        ((REAL_NAME(F) == NULL ?                                               \
              (REAL_NAME(F) =                                                  \
                   (__typeof(REAL_NAME(F)))dlvsym(RTLD_NEXT, #F, V)) :         \
              0),                                                              \
         REAL_NAME(F)(__VA_ARGS__))
    /* Fix for glibc pthread_cond version issues. We assume that the SUT expects
     * glibc 2.3.2 or above. For more information see
     * https://blog.fesnel.com/blog/2009/08/25/preloading-with-multiple-symbol-versions/
     * or issue #36
     */
    #define REALP(F, ...) REAL_CALL_VERSION(F, "GLIBC_2.3.2", __VA_ARGS__)
#else
    #define REALP(...) REAL(__VA_ARGS__)
#endif
#endif /* DICE_INTERPOSE_H */
