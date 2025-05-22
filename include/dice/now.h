/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef _NOW_H_
#define _NOW_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef uint64_t nanosec_t;

#define NOW_NANOSECOND  ((nanosec_t)1)
#define NOW_MICROSECOND (1000 * NOW_NANOSECOND)
#define NOW_MILLISECOND (1000 * NOW_MICROSECOND)
#define NOW_SECOND      (1000 * NOW_MILLISECOND)

static inline nanosec_t
now(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
        perror("could not get clock time");
        exit(EXIT_FAILURE);
    }

    const nanosec_t seconds_ns     = ((nanosec_t)ts.tv_sec) * NOW_SECOND;
    const nanosec_t nanoseconds_ns = (nanosec_t)ts.tv_nsec;
    const nanosec_t result_ns      = seconds_ns + nanoseconds_ns;

    return result_ns;
}

static inline double
in_sec(nanosec_t ts)
{
    return ((double)ts) / NOW_SECOND;
}

static inline struct timespec
to_timespec(nanosec_t ts)
{
    return (struct timespec){
        .tv_sec  = ts / NOW_SECOND,
        .tv_nsec = ts % NOW_SECOND,
    };
}

#endif /* _NOW_H_ */
