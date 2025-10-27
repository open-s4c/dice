/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_CAPTURE_H
#define DICE_CAPTURE_H

/* The Self module subscribes for all events from INTERCEPT_* chains and
 * republishes them the in following chains with additional "self" metadata such
 * as ID and TLS pointers. */
#define CAPTURE_EVENT  4
#define CAPTURE_BEFORE 5
#define CAPTURE_AFTER  6

#endif /* DICE_CAPTURE_H */
