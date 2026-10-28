/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_CAPTURE_H
#define DICE_CAPTURE_H

#include <dice/compiler.h>

/* The Self module subscribes for all events from CHAIN_INTERCEPT_* chains and
 * republishes them the in following chains with additional "self" metadata such
 * as ID and TLS pointers. */
#ifndef DICE_ROLLED
    #define CHAIN_CAPTURE_EVENT  4
    #define CHAIN_CAPTURE_BEFORE 5
    #define CHAIN_CAPTURE_AFTER  6
#endif

/* Existing sources retain their public chain names. */
#ifndef CAPTURE_EVENT
    #define CAPTURE_EVENT CHAIN_CAPTURE_EVENT
#endif
#ifndef CAPTURE_BEFORE
    #define CAPTURE_BEFORE CHAIN_CAPTURE_BEFORE
#endif
#ifndef CAPTURE_AFTER
    #define CAPTURE_AFTER CHAIN_CAPTURE_AFTER
#endif

#endif /* DICE_CAPTURE_H */
