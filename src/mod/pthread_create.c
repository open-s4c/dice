/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>
#include <stdlib.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/events/thread.h>
#include <dice/interpose.h>
#include <dice/mempool.h>
#include <dice/module.h>
#include <dice/pubsub.h>

typedef struct {
    void *(*run)(void *);
    void *arg;
} trampoline_t;

DICE_NORET
INTERPOSE(void, pthread_exit, void *ptr)
{
    struct pthread_exit_event ev = {
        .pc   = INTERPOSE_PC,
        .ptr  = ptr,
        .func = REAL_FUNC(pthread_exit),
    };
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_THREAD_EXIT, &ev, 0);
    ev.func(ptr);
    exit(1); // unreachable
}

static void *
trampoline_(void *targ)
{
    trampoline_t *t      = (trampoline_t *)targ;
    void *arg            = t->arg;
    void *(*run)(void *) = t->run;
    mempool_free(t);

    PS_PUBLISH(INTERCEPT_EVENT, EVENT_THREAD_START, 0, 0);
    void *ret = run(arg);
    PS_PUBLISH(INTERCEPT_EVENT, EVENT_THREAD_EXIT, 0, 0);
    return ret;
}

INTERPOSE(int, pthread_create, pthread_t *thread, const pthread_attr_t *attr,
          void *(*run)(void *), void *arg)
{
    trampoline_t *t;

    t  = mempool_alloc(sizeof(trampoline_t));
    *t = (trampoline_t){.arg = arg, .run = run};

    struct pthread_create_event ev = {
        .pc     = INTERPOSE_PC,
        .thread = thread,
        .attr   = attr,
        .run    = run,
        .arg    = arg,
        .func   = REAL_FUNC(pthread_create),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_THREAD_CREATE, &ev, &md);
    ev.ret = ev.func(thread, attr, trampoline_, t);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_THREAD_CREATE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, pthread_join, pthread_t thread, void **ptr)
{
    struct pthread_join_event ev = {
        .pc     = INTERPOSE_PC,
        .thread = thread,
        .ptr    = ptr,
        .func   = REAL_FUNC(pthread_join),
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_THREAD_JOIN, &ev, &md);
    ev.ret = ev.func(thread, ptr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_THREAD_JOIN, &ev, &md);

    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_THREAD_START)
PS_ADVERTISE_TYPE(EVENT_THREAD_EXIT)
PS_ADVERTISE_TYPE(EVENT_THREAD_CREATE)
PS_ADVERTISE_TYPE(EVENT_THREAD_JOIN)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
