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

int init_called;
int fini_called;
int run_called;

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_INIT, { init_called++; })
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_FINI, { fini_called++; })

void *
run()
{
    run_called++;
    pthread_exit(0);
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);

    assert(run_called == 1);
    assert(init_called == 1);
    assert(fini_called == 1);

    return 0;
}
