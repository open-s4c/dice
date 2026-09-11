/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/interpose.h>
#include <dice/module.h>

INTERPOSE(int, pthread_key_create,
          pthread_key_t *key,
          void (*destr_function)(void *))
{
    struct pthread_key_create_event ev = {
        .pc            = INTERPOSE_PC,
        .key           = key,
        .destr_function = destr_function,
        .ret           = 0,
        .func          = REAL_FUNC(pthread_key_create),
    };

    metadata_t md = {0};

    PS_PUBLISH(
        INTERCEPT_BEFORE,
        EVENT_PTHREAD_KEY_CREATE,
        &ev,
        &md
    );

    ev.ret = ev.func(ev.key, ev.destr_function);

    PS_PUBLISH(
        INTERCEPT_AFTER,
        EVENT_PTHREAD_KEY_CREATE,
        &ev,
        &md
    );

    return ev.ret;
}

INTERPOSE(int, pthread_key_delete, pthread_key_t key)
{
    struct pthread_key_delete_event ev = {
        .pc   = INTERPOSE_PC,
        .key  = key,
        .ret  = 0,
        .func = REAL_FUNC(pthread_key_delete),
    };

    metadata_t md = {0};

    PS_PUBLISH(
        INTERCEPT_BEFORE,
        EVENT_PTHREAD_KEY_DELETE,
        &ev,
        &md
    );

    ev.ret = ev.func(ev.key);

    PS_PUBLISH(
        INTERCEPT_AFTER,
        EVENT_PTHREAD_KEY_DELETE,
        &ev,
        &md
    );

    return ev.ret;
}
/*
INTERPOSE(int, pthread_setspecific,
          pthread_key_t key,
          const void *pointer)
{
    struct pthread_setspecific_event ev = {
        .pc      = INTERPOSE_PC,
        .key     = key,
        .pointer = pointer,
        .ret     = 0,
        .func    = REAL_FUNC(pthread_setspecific),
    };

    metadata_t md = {0};

    PS_PUBLISH(
        INTERCEPT_BEFORE,
        EVENT_PTHREAD_SETSPECIFIC,
        &ev,
        &md
    );

    ev.ret = ev.func(ev.key, ev.pointer);

    PS_PUBLISH(
        INTERCEPT_AFTER,
        EVENT_PTHREAD_SETSPECIFIC,
        &ev,
        &md
    );

    return ev.ret;
}

INTERPOSE(void *, pthread_getspecific, pthread_key_t key)
{
    struct pthread_getspecific_event ev = {
        .pc   = INTERPOSE_PC,
        .key  = key,
        .ret  = NULL,
        .func = REAL_FUNC(pthread_getspecific),
    };

    metadata_t md = {0};

    PS_PUBLISH(
        INTERCEPT_BEFORE,
        EVENT_PTHREAD_GETSPECIFIC,
        &ev,
        &md
    );

    ev.ret = ev.func(ev.key);

    PS_PUBLISH(
        INTERCEPT_AFTER,
        EVENT_PTHREAD_GETSPECIFIC,
        &ev,
        &md
    );

    return ev.ret;
}
*/
/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_PTHREAD_KEY_CREATE)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_KEY_DELETE)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_SETSPECIFIC)
PS_ADVERTISE_TYPE(EVENT_PTHREAD_GETSPECIFIC)

DICE_MODULE_INIT()
