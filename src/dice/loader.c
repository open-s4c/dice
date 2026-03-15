/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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
#define DICE_DSO            "DICE_DSO"
#define DICE_PLUGIN_MODULES "DICE_PLUGIN_MODULES"

int ps_dispatch_max(void);
void ps_init_();

#ifndef DICE_DISABLE_PUBUSB_CTOR_INIT
/* Force pubsub init in a constructor without priority, ensuring this
 * constructor is last */
static void __attribute__((constructor))
init_()
{
    ps_init_();
}
#endif

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
    const char *envvar     = getenv(DICE_PLUGIN_MODULES);
    const int from_preload = (envvar == NULL || envvar[0] == '\0');
    if (from_preload)
        envvar = getenv(PRELOAD);
    log_debug("[%4d] LOAD: builtin modules: 0..%d", DICE_MODULE_SLOT,
              ps_dispatch_max());
    if (envvar != NULL && envvar[0] != '\0') {
        char *plugins = strdup_(envvar);
        if (plugins == NULL)
            log_fatal("could not duplicate string: %s", envvar);

        char *path = strtok(plugins, ":");
        if (path == NULL)
            log_fatal("string tokenizer failed: %s", plugins);

        // we now dlopen every preloaded library to ensure the constructors are
        // all called before the first event is published. However, we shouldnt
        // try to dlopen the main dice library, which holds the current code
        // because some user constructors might not be idempotent. Typically the
        // main dice library is the first preloaded library. If that is not the
        // case, we can use the envvar DICE_DSO to identify the exact name
        // passed to the PRELOAD.
        envvar = getenv(DICE_DSO);
        if (envvar == NULL && from_preload)
            // skip first library only for PRELOAD-based lists
            path = strtok(NULL, ":");

        while (path != NULL) {
            if (envvar == NULL || strcmp(path, envvar) != 0)
                load_plugin_(path);
            path = strtok(NULL, ":");
        }
        mempool_free(plugins);
    }
    log_debug("[%4d] DONE: %s", DICE_MODULE_SLOT, __FILE__);
})
#endif
