/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/module.h>
#include <dice/self.h>

#define NTHREADS 3
vatomic32_t exists[NTHREADS];

PS_SUBSCRIBE(CAPTURE_EVENT, EVENT_SELF_INIT, {
    thread_id id = self_id(md);
    assert(id > 0);
    // ids start with 1
    uint32_t count = vatomic_get_inc(exists + id - 1);
    assert(count == 0);
})

void *
run(void *arg)
{
    (void)arg;
    // send some event to trigger SELF_INIT
    PS_PUBLISH(INTERCEPT_EVENT, 100, 0, 0);
    return NULL;
}


int
main(void)
{
    // we'll create NTHREADS-1 threads as the main thread counts as one.
    pthread_t t[NTHREADS - 1];

    // send some event to trigger SELF_INIT of main thread
    PS_PUBLISH(INTERCEPT_EVENT, 100, 0, 0);

    for (int i = 0; i < NTHREADS - 1; i++)
        pthread_create(&t[i], 0, run, 0);

    for (int i = 0; i < NTHREADS - 1; i++)
        pthread_join(t[i], 0);

    // here we check that every thread (including the main thread) has received
    // SELF_INIT once and only once.
    for (int i = 0; i < NTHREADS; i++)
        assert(vatomic_read(exists + i) == 1);

    return 0;
}
