/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <cstdio>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

extern "C" {
#include <trace_checker.h>

#include <dice/chains/capture.h>
#include <dice/events/memaccess.h>
#include <dice/events/pthread.h>
#include <dice/events/self.h>
#include <dice/events/stacktrace.h>
#include <dice/events/thread.h>
}

enum State : uint8_t { NotStarted, Claimed, Done };
std::mutex queueLock_;

void *
once_plus_one(void *ptr)
{
    uint8_t &at = *(uint8_t *)ptr;

    queueLock_.lock();
    auto state = at;
    queueLock_.unlock();
    if (state == Done) {
        return NULL;
    }
    queueLock_.lock();
    state = at;

    if (state == NotStarted) {
        at = Claimed;
        queueLock_.unlock();
        // sleep(1);
        queueLock_.lock();
        at = Done;
        queueLock_.unlock();
        return NULL;
    }
    queueLock_.unlock();
    while (state != Done) {
        /*spin*/
        queueLock_.lock();
        state = at;
        queueLock_.unlock();
    }
    return NULL;
}

#define NUM_THREADS 1

struct expected_event expected_1[] = {
#if defined(__linux__) && !defined(__GLIBC__)
    // on Alpine , multiple allocations occur before SELF_INIT
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_START),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_STACKTRACE_ENTER),
#elif defined(__linux__)
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_START),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_STACKTRACE_ENTER),
#elif defined(__NetBSD__)
    // initialization until THREAD_START is published
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_THREAD_START),
#endif
    // main starts
    // now in the main function, write notStarted, create thread
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_MA_WRITE),
    EXPECTED_EVENT(CAPTURE_BEFORE, EVENT_THREAD_CREATE),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_THREAD_CREATE),
    // depending compiler/optimization, pthread variable might be read once
    EXPECTED_SOME(CAPTURE_EVENT, EVENT_MA_READ, 0, 1),
    EXPECTED_EVENT(CAPTURE_BEFORE, EVENT_THREAD_JOIN),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_THREAD_JOIN),

    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_STACKTRACE_EXIT),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_THREAD_EXIT),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),

#if defined(__linux__)
    EXPECTED_END,
#elif defined(__NetBSD__)
    EXPECTED_ANY_SUFFIX,
#endif
};

struct expected_event expected_2[] = {
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_SELF_INIT),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_START),
    // enter main function
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_STACKTRACE_ENTER),

    // first critical section
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_MA_READ),
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_UNLOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_UNLOCK),

    // second critical section
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_MA_READ),
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_MA_WRITE),
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_UNLOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_UNLOCK),

    // third critical section
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_LOCK),
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_MA_WRITE),
    EXPECTED_SUFFIX(CAPTURE_BEFORE, EVENT_PTHREAD_MUTEX_UNLOCK),
    EXPECTED_EVENT(CAPTURE_AFTER, EVENT_PTHREAD_MUTEX_UNLOCK),

    EXPECTED_SOME(CAPTURE_EVENT, EVENT_STACKTRACE_EXIT, 1, 3),
    //  leave main function
    EXPECTED_EVENT(CAPTURE_EVENT, EVENT_THREAD_EXIT),
    // some cleanup
    EXPECTED_SUFFIX(CAPTURE_EVENT, EVENT_SELF_FINI),
    EXPECTED_END,
};

int
main()
{
    register_expected_trace(1, expected_1);
    register_expected_trace(2, expected_2);
    uint8_t at = NotStarted;
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(threads + i, NULL, once_plus_one, (void *)&at);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(*(threads + i), NULL);
    }
    return 0;
}
