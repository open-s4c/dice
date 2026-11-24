/* SPDX-License-Identifier: 0BSD */
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/events/pthread.h>
#include <dice/module.h>

static uintptr_t trampoline_address;

static void *
worker(void *arg)
{
    return arg;
}

static int
fail_create(pthread_t *thread, const pthread_attr_t *attr,
            void *(*start)(void *), void *arg)
{
    (void)thread;
    (void)attr;
    (void)start;
    trampoline_address = (uintptr_t)arg;
    return EAGAIN;
}

PS_SUBSCRIBE(CHAIN_INTERCEPT_BEFORE, EVENT_THREAD_CREATE, {
    struct pthread_create_event *ev = event;
    if (ev->run == worker)
        ev->func = fail_create;
})

int
main(void)
{
    pthread_t thread;
    ensure(pthread_create(&thread, NULL, worker, NULL) == EAGAIN);
    uintptr_t first = trampoline_address;
    ensure(first != 0);

    /* The pool reuses the block released by the previous failed create. */
    trampoline_address = 0;
    ensure(pthread_create(&thread, NULL, worker, NULL) == EAGAIN);
    ensure(trampoline_address == first);
    return 0;
}
