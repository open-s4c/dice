/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/events/thread.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <vsync/atomic.h>

int init_called;
int fini_called;
int run_called;

vatomic32_t done;
vatomic32_t start;

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_INIT, { init_called++; })
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_FINI, {
    fini_called++;
    vatomic_inc(&done);
})

void *
run()
{
    vatomic_await_neq(&start, 0);
    run_called++;
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_detach(t);
    vatomic_inc(&start);
    vatomic_await_neq(&done, 0);

    assert(run_called == 1);
    assert(init_called == 1);
    assert(fini_called == 1);

    return 0;
}
