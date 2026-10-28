/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#ifndef DICE_CHAIN_NAMES_H
#define DICE_CHAIN_NAMES_H

#include <string.h>

/* Keep the established diagnostic names and accept both advertisement and
 * lookup spellings when old and new module sources are linked together. */
static inline const char *
ps_chain_name_normalize_(const char *name)
{
    if (name && !strcmp(name, "CHAIN_DICE_CONTROL"))
        return "CHAIN_CONTROL";
    static const char *const aliases[] = {
        "INTERCEPT_EVENT", "INTERCEPT_BEFORE", "INTERCEPT_AFTER",
        "CAPTURE_EVENT",   "CAPTURE_BEFORE",   "CAPTURE_AFTER",
    };
    if (name && !strncmp(name, "CHAIN_", 6))
        for (size_t i = 0; i < sizeof(aliases) / sizeof(aliases[0]); i++)
            if (!strcmp(name + 6, aliases[i]))
                return name + 6;
    return name;
}

#endif /* DICE_CHAIN_NAMES_H */
