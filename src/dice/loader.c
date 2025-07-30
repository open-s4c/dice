/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DICE_MODULE_PRIO 2
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
void ps_init_();

static DICE_CTOR void
_init()
{
    ps_init_();
}

static void
_load_plugin(const char *path)
{
    log_printf("[%4d] LOAD: %s\n", DICE_MODULE_PRIO, path);
    void *handle = dlopen(path, RTLD_GLOBAL | RTLD_LAZY);
    char *err    = dlerror();
    if (!handle)
        log_fatalf("could not open %s: %s\n", path, err);
}

static char *
_strdup(const char *str)
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
    log_printf("[%4d] INIT: %s ...\n", DICE_MODULE_PRIO, __FILE__);
    const char *envvar = getenv(PRELOAD);
    log_printf("[%4d] LOAD: builtin modules: 0..%d\n", DICE_MODULE_PRIO,
               ps_dispatch_max());
    if (envvar != NULL) {
        char *plugins = _strdup(envvar);
        assert(plugins);

        char *path = strtok(plugins, ":");
        assert(path);
        // skip first
        path = strtok(NULL, ":");
        while (path != NULL) {
            _load_plugin(path);
            path = strtok(NULL, ":");
        }
        mempool_free(plugins);
    }
    log_printf("[%4d] DONE: %s\n", DICE_MODULE_PRIO, __FILE__);
})
