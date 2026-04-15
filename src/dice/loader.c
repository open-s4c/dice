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
#define DICE_EXCLUDE_DSO    "DICE_EXCLUDE_DSO"
#define DICE_PLUGIN_MODULES "DICE_PLUGIN_MODULES"

int ps_dispatch_max(void);
void ps_init_();

#ifndef DICE_DISABLE_PUBSUB_CTOR_INIT
/* Force pubsub init in a constructor without priority, ensuring this
 * constructor is last */
static void __attribute__((constructor)) init_()
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
    static bool done_ = false;
    if (done_)
        return PS_OK;
    done_ = true;

    log_debug("[%4d] INIT: %s ...", DICE_MODULE_SLOT, __FILE__);
    const char *dso        = getenv(DICE_EXCLUDE_DSO);
    const char *pluginvar  = getenv(DICE_PLUGIN_MODULES);
    const char *preloadvar = getenv(PRELOAD);
    const char *envvar     = pluginvar != NULL ? pluginvar : preloadvar;
    log_debug("[%4d] LOAD: builtin modules: 0..%d", DICE_MODULE_SLOT,
              ps_dispatch_max());

    log_debug("[%4d] %s: %s", DICE_MODULE_SLOT, PRELOAD, preloadvar);
    log_debug("[%4d] DICE_PLUGIN_MODULES: %s", DICE_MODULE_SLOT, pluginvar);
    log_debug("[%4d] DICE_EXCLUDE_DSO%s: %s", DICE_MODULE_SLOT,
              pluginvar == NULL ? "" : " (ignored)", dso);

    if (envvar != NULL && envvar[0] != '\0') {
        log_debug("[%4d] plugin modules: %s", DICE_MODULE_SLOT, envvar);
        char *plugins = strdup_(envvar);
        if (plugins == NULL)
            log_fatal("could not duplicate string: %s", envvar);

        char *path = strtok(plugins, ":");
        if (path == NULL)
            log_fatal("string tokenizer failed: %s", plugins);

        // We dlopen every module in the selected list to ensure constructors
        // run before the first Dice event is published.
        //
        // Source selection policy:
        // - If DICE_PLUGIN_MODULES is set, it is authoritative and we load
        //   exactly those entries.
        // - Otherwise we use PRELOAD (LD_PRELOAD or
        //   DYLD_INSERT_LIBRARIES).
        //
        // Exclusion policy (PRELOAD fallback only):
        // - DICE_EXCLUDE_DSO identifies one library to skip while loading.
        // - If DICE_EXCLUDE_DSO is empty/unset, default it to the first PRELOAD
        //   entry (typically the main Dice runtime dso) so we avoid
        //   re-dlopening it.
        if (pluginvar == NULL && (dso == NULL || dso[0] == '\0'))
            dso = path;

        while (path != NULL) {
            if (pluginvar != NULL || dso == NULL || strcmp(path, dso) != 0)
                load_plugin_(path);
            else
                log_debug("[%4d] SKIP: %s", DICE_MODULE_SLOT, path);
            path = strtok(NULL, ":");
        }
        mempool_free(plugins);
    }
    log_debug("[%4d] DONE: %s", DICE_MODULE_SLOT, __FILE__);
})
#endif
