/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef BINGO_REAL_H
#define BINGO_REAL_H

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

#include <bingo/module.h>

#if defined(__linux__) || defined(__NetBSD__)
    #include <dlfcn.h>

    #define REAL(F, ...)                                                       \
        ((REAL_NAME(F) == NULL ? (REAL_NAME(F) = dlsym(RTLD_NEXT, #F)) : 0),   \
         REAL_NAME(F)(__VA_ARGS__))
    #define REAL_NAME(F) __tilt_real_##F

    #define INTERPOSE(T, F, ...)                                               \
        T F(__VA_ARGS__);                                                      \
        static T (*REAL_NAME(F))(__VA_ARGS__);                                 \
        T F(__VA_ARGS__)

#elif defined(__APPLE__)

    #define REAL(F, ...) F(__VA_ARGS__)
    #define FAKE_NAME(F) __tilt_fake_##F

    #define INTERPOSE(T, F, ...)                                               \
        T FAKE_NAME(F)(__VA_ARGS__);                                           \
        static struct {                                                        \
            const void *fake;                                                  \
            const void *real;                                                  \
        } _tilt_interpose_##F                                                  \
            __attribute__((used, section("__DATA,__interpose"))) = {           \
                (const void *)&FAKE_NAME(F), (const void *)&F};                \
        T FAKE_NAME(F)(__VA_ARGS__)
#else
    #error Unsupported platform
#endif

#endif /* BINGO_REAL_H */
