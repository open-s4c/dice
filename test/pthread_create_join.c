/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/ensure.h>
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/events/pthread.h>
#include <dice/events/thread.h>
#include <dice/module.h>
#include <dice/pubsub.h>

int init_called;
int fini_called;
int run_called;

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_START, { init_called++; })
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_EXIT, { fini_called++; })

void *
run()
{
    run_called++;
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);

    ensure(run_called == 1);
    ensure(init_called == 1);
    ensure(fini_called == 1);

    return 0;
}
