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

#if !(defined(__linux__) || defined(__NetBSD__) || defined(__APPLE__))
    #error Unsupported platform
#endif

void *real_sym(const char *, const char *);

/* REAL_NAME(F) is the function pointer for the real function F. */
#define REAL_NAME(F) _dice_real_##F

/* REAL_DECL(T, F, ...) declares the function pointer to the real function
 * F, with return type T and arguments as varargs. */
#define REAL_DECL(T, F, ...) DICE_HIDE DICE_WEAK T (*REAL_NAME(F))(__VA_ARGS__);

/* REAL(F, ...) calls the real function F using its declared function
 * pointer. */
#if defined(__linux__)

    #define REAL(F, ...) REAL_CALL(F, 0, ##__VA_ARGS__)
    /* Fix for glibc pthread_cond version issues. We assume that the SUT
     * expects glibc 2.3.2 or above. For more information see issue #36 or
     * https://blog.fesnel.com/blog/2009/08/25/preloading-with-multiple-symbol-versions/
     */
    #define REALP(F, ...) REAL_CALL(F, "GLIBC_2.3.2", __VA_ARGS__)

#elif defined(__NetBSD__)

    #define REAL(F, ...)  REAL_CALL(F, 0, ##__VA_ARGS__)
    #define REALP(F, ...) REAL_CALL(F, 0, ##__VA_ARGS__)

#elif defined(__APPLE__)
    /* On macOS, however, the mechanism uses the interposition attribute and
     * the call to the real function can be done directly. */
    #define REAL(F, ...) F(__VA_ARGS__)
#endif

/* INTERPOSE(T, F, ...) { ... } defines an interposition function for F
 * using the platform-specific mechanism */
#if defined(__linux__) || defined(__NetBSD__)

    #define INTERPOSE(T, F, ...)                                               \
        T F(__VA_ARGS__);                                                      \
        REAL_DECL(T, F, __VA_ARGS__);                                          \
        T F(__VA_ARGS__)

#elif defined(__APPLE__)

    #define INTERPOSE(T, F, ...)                                               \
        T FAKE_NAME(F)(__VA_ARGS__);                                           \
        static struct {                                                        \
            const void *fake;                                                  \
            const void *real;                                                  \
        } _dice_interpose_##F                                                  \
            __attribute__((used, section("__DATA,__interpose"))) = {           \
                (const void *)&FAKE_NAME(F), (const void *)&F};                \
        T FAKE_NAME(F)(__VA_ARGS__)

    #define FAKE_NAME(F) _dice_fake_##F

#endif

/* REAL_CALL(F, ...) calls a declared real function. In case the
 * function pointer is NULL, this macro initializes it using
 * `REAL_SYM()`. If DICE_TEST_INTERPOSE is defined, the function pointer
 * is not set and REAL_SYM is called for every call of REAL_CALL. */
#if !defined(DICE_TEST_INTERPOSE)
    #define REAL_CALL(F, V, ...)                                               \
        ((REAL_NAME(F) == NULL ? (REAL_NAME(F) = REAL_SYM(F, V)) : 0),         \
         REAL_NAME(F)(__VA_ARGS__))
#else
    #define REAL_CALL(F, V, ...)                                               \
        ((__typeof(REAL_NAME(F)))REAL_SYM(F, V))(__VA_ARGS__)
#endif

/* REAL_SYM(F, V) finds real symbol of declared function F with version
 * V. The version value V can be NULL. */
#define REAL_SYM(F, V) (__typeof(REAL_NAME(F)))real_sym(#F, V)

static inline void *
_real_sym(const char *name, const char *ver)
{
#if defined(__glibc__)
    if (ver != NULL)
        return dlvsym(RTLD_NEXT, name, ver);
#endif
    (void)ver;
    return dlsym(RTLD_NEXT, name);
}

/* Finds a real function by calling dlsym or dlvsym. */
#if !defined(DICE_TEST_INTERPOSE)
DICE_HIDE DICE_WEAK void *
real_sym(const char *name, const char *ver)
{
    return _real_sym(name, ver);
}
#endif

#endif /* DICE_INTERPOSE_H */
