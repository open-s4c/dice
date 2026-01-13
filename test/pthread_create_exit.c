/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <pthread.h>

#include <dice/chains/intercept.h>
#include <dice/ensure.h>
#include <dice/events/pthread.h>
#include <dice/events/thread.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <vsync/atomic.h>

vatomic32_t start_called;
vatomic32_t exit_called;
vatomic32_t run_called;

PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_START,
             { vatomic_inc(&start_called); })
PS_SUBSCRIBE(INTERCEPT_EVENT, EVENT_THREAD_EXIT, { vatomic_inc(&exit_called); })

void *
run()
{
    vatomic_inc(&run_called);
    pthread_exit(0);
    return 0;
}

int
main()
{
    pthread_t t;
    pthread_create(&t, 0, run, 0);
    pthread_join(t, 0);

    ensure(vatomic_read(&run_called) == 1);
    ensure(vatomic_read(&start_called) == 1);
    ensure(vatomic_read(&exit_called) == 1);

    return 0;
}
