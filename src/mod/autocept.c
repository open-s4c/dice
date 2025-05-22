/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#if !defined(__APPLE__)
    #include <dlfcn.h>
#endif

#include "autocept.h"
#include "defs.h"
#include <bingo/intercept.h>
#include <bingo/module.h>

BINGO_MODULE_INIT()

// TODO: add function cache

void *
autocept_before(const char *name)
{
    autocept_event_t ev = {.fname = name};
    metadata_t md       = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_AUTOCEPT, &ev, &md);
#if defined(__APPLE__)
    return 0;
#else
    return dlsym(RTLD_NEXT, name);
#endif
}

void
autocept_after(void)
{
    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_AUTOCEPT, 0, &md);
}
