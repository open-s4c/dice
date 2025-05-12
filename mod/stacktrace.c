/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * @file stacktrace.c
 * @brief Publishes func_entry and func_exit events from TSAN instrumentation.
 *
 ******************************************************************************/
#include <stdint.h>

#include <bingo/intercept/stacktrace.h>
#include <bingo/interpose.h>
#include <bingo/module.h>

BINGO_MODULE_INIT()

void
__tsan_func_entry(void *caller)
{
    stacktrace_event_t ev = {.caller = caller, .pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_STACKTRACE_ENTER, &ev, &md);
}
void
__tsan_func_exit(void)
{
    stacktrace_event_t ev = {.pc = INTERPOSE_PC};

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_STACKTRACE_EXIT, &ev, &md);
}
