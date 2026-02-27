/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DICE_MODULE_SLOT 1
#include <dice/events/dice.h>
#include <dice/log.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>

#if defined(__APPLE__)
    #define PRELOAD "DYLD_INSERT_LIBRARIES"
#else
    #define PRELOAD "LD_PRELOAD"
#endif

int ps_dispatch_max(void);

#ifndef DICE_DISABLE_LOADER
static void
load_plugin_(const char *path)
{
    log_debug("[%4d] LOAD: %s", DICE_MODULE_SLOT, path);
    void *handle = dlopen(path, RTLD_GLOBAL | RTLD_LAZY);
    char *err    = dlerror();
    if (!handle)
        log_fatal("could not open %s: %s", path, err);
}

static char *
strdup_(const char *str)
{
    if (str == NULL)
        return NULL;

    const size_t len = strlen(str) + 1;
    char *copy       = mempool_alloc(len);
    if (copy == NULL)
        return copy;

    return strncpy(copy, str, len);
}

PS_SUBSCRIBE(CHAIN_CONTROL, EVENT_DICE_INIT, {
    log_debug("[%4d] INIT: %s ...", DICE_MODULE_SLOT, __FILE__);
    const char *envvar = getenv(PRELOAD);
    log_debug("[%4d] LOAD: builtin modules: 0..%d", DICE_MODULE_SLOT,
              ps_dispatch_max());
    if (envvar != NULL) {
        char *plugins = strdup_(envvar);
        assert(plugins);

        char *path = strtok(plugins, ":");
        assert(path);
        // skip first
        path = strtok(NULL, ":");
        while (path != NULL) {
            load_plugin_(path);
            path = strtok(NULL, ":");
        }
        mempool_free(plugins);
    }
    log_debug("[%4d] DONE: %s", DICE_MODULE_SLOT, __FILE__);
})
#endif
