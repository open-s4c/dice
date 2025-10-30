/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/capture.h>
#include <dice/events/malloc.h>
#include <dice/events/pthread.h>
#include <dice/events/self.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>

static int exit_called;

// TLS key and once guard
static pthread_key_t key;
static pthread_once_t once;
static void *arg_check; // keep arg in a variable to double check later

// A once-initializer to make Alpine Linux happy. This is called in the second
// thread and in the main thread but the compilation on Alpine doesn't like it.
static void dtor(void *arg);
static void
tls_init()
{
    if (pthread_key_create(&key, dtor) != 0)
        abort();
}


// run function of thread: allocate some memory and put it in TLS
static void *
run(void *_)
{
    (void)_;
    pthread_once(&once, tls_init);
    arg_check = malloc(123);
    if (pthread_setspecific(key, (const void *)arg_check) != 0)
        abort();
    log_printf("1) tid = %p\n", (void *)pthread_self());
    return 0;
}

// a destructor will free the memory allocated by the thread
static void
dtor(void *arg)
{
    log_printf("3) tid = %p (no self in dtor)\n", (void *)pthread_self());
    log_printf("\tfree(%p)\n", arg);
    free(arg);
}

int
main()
{
    pthread_t t;
    pthread_once(&once, tls_init);
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);
    return 0;
}

// remember when thread exit
PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_THREAD_EXIT, {
    log_printf("2) tid = %p\tself = %p\n", (void *)pthread_self(), md);
    log_printf("Thread %" PRIu64 " exit\n", self_id(md));
    assert(self_id(md) == 2);
    exit_called++;
})

// if retired thread (after THREAD_EXIT) calls free(), we should see a
// CAPTURE_BEFORE EVENT_FREE
PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_FREE, {
    if (exit_called == 0)
        return PS_OK;

    struct free_event *ev = EVENT_PAYLOAD(ev);
    if (ev->ptr != arg_check)
        return PS_OK;

    log_printf("4) tid = %p\tself = %p\n", (void *)pthread_self(), md);
    log_printf("Retired thread before free. ID should be 2\n");
    log_printf("\tuid = %" PRIu64 "\n", self_id(md));
    assert(self_id(md) == 2);
})
