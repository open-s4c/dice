/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_TEST_PHASE1_ALLOC_H
#define DICE_TEST_PHASE1_ALLOC_H

#define PHASE1_STRDUP_ARG      "phase1_strdup"
#define PHASE1_STRNDUP_ARG     "phase1_strndup"
#define PHASE1_STRNDUP_N       7
#define PHASE1_WCSDUP_ARG      L"phase1_wcsdup"
#define PHASE1_TMPDIR_PREFIX   "dice-phase1."
#define PHASE1_TMPDIR_TEMPLATE "/tmp/dice-phase1.XXXXXX"
#define PHASE1_GETCWD_SIZE     128
#define PHASE1_REALPATH_ARG    "phase1-realpath-target"
#define PHASE1_TEMPNAM_PFX     "p1tmp"
#define PHASE1_ASPRINTF_FMT    "phase1-asprintf-%d"
#define PHASE1_ASPRINTF_ARG    7
#define PHASE1_VASPRINTF_FMT   "phase1-vasprintf-%d"
#define PHASE1_VASPRINTF_ARG   9
#define PHASE1_CPUSET_COUNT    65

#endif /* DICE_TEST_PHASE1_ALLOC_H */
