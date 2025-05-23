/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
/*******************************************************************************
 * Switcher is a sort of barrier where threads get blocked until it's their
 * turn. The interface is basically yield() to block a thread and wake() to
 * enable a specific thread. The thread enabled does not necessarily has to be
 * in the switcher at the moment it becomes enabled (it may arrive later).
 ******************************************************************************/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
// clang-format off
#define FUTEX_USERSPACE
#include <vsync/thread/mutex.h>
#include <vsync/thread/cond.h>
// clang-format on

#define DICE_XTOR_PRIO 198
#include <dice/log.h>
#include <dice/module.h>
#include <dice/switcher.h>

DICE_MODULE_INIT()

#ifndef SWITCHER_LOG
    #undef log_debugf
    #define log_debugf(...)                                                    \
        do {                                                                   \
        } while (0)
#endif

typedef struct {
    vmutex_t mutex;
    vcond_t cnd[SWITCHER_NBUCKETS];
    int cnd_counter[SWITCHER_NBUCKETS];
    thread_id next;
    thread_id prev;
    int status;
    nanosec_t slack;
} switcher_t;

static switcher_t _switcher;

void
_switcher_resuming(void)
{
}

int
switcher_yield(thread_id id, bool any)
{
    thread_id prev, next;
    log_debugf("\t\t\t\tYIELD  thread %" PRIu64 "\n", id);

    vmutex_acquire(&_switcher.mutex);
    next = _switcher.next;

    int bucket = id % SWITCHER_NBUCKETS;

    // before the slack time passes, we still have a chance to continue
    if (_switcher.slack && _switcher.prev == id) {
        next = id;
    }

    _switcher.cnd_counter[bucket]++;

    while (next != id && _switcher.status != SWITCHER_ABORTED &&
           // if any == false, the next condition is disabled
           (!any || next != ANY_THREAD)) {
        vcond_signal(&_switcher.cnd[bucket]);
        vcond_wait(&_switcher.cnd[bucket], &_switcher.mutex);
        if (_switcher.slack) {
            struct timespec ts = to_timespec(_switcher.slack);
            vmutex_release(&_switcher.mutex);
            nanosleep(&ts, NULL);
            vmutex_acquire(&_switcher.mutex);
            _switcher.slack = 0;
        }
        next = _switcher.next;
    }

    _switcher.cnd_counter[bucket]--;

    if (_switcher.status == SWITCHER_ABORTED) {
        vmutex_release(&_switcher.mutex);
        return SWITCHER_ABORTED;
    }

    prev            = _switcher.prev;
    _switcher.prev  = id;
    _switcher.next  = NO_THREAD;
    _switcher.slack = 0;

    int status;

    _switcher.status =
        prev != id || next == ANY_THREAD ? SWITCHER_CHANGED : SWITCHER_CONTINUE;

    status = _switcher.status;

    vmutex_release(&_switcher.mutex);

    _switcher_resuming();
    log_debugf("\t\t\t\tRESUME thread %" PRIu64 "\n", id);

    return status;
}

void
switcher_wake(thread_id id, nanosec_t slack)
{
    log_debugf("\t\t\t\tWAKE   thread %" PRIu64 "\n", id);

    vmutex_acquire(&_switcher.mutex);
    assert(_switcher.next == NO_THREAD);
    assert(id != NO_THREAD);

    int bucket = id % SWITCHER_NBUCKETS;

    _switcher.next  = id;
    _switcher.slack = slack;
    if (id == ANY_THREAD) {
        for (int i = 0; i < SWITCHER_NBUCKETS; i++) {
            if (_switcher.cnd_counter[i] > 0) {
                vcond_signal(&_switcher.cnd[i]);
            }
        }
    } else {
        vcond_signal(&_switcher.cnd[bucket]);
    }
    vmutex_release(&_switcher.mutex);
}

void
switcher_abort()
{
    log_debugf("ABORT called\n");
    vmutex_acquire(&_switcher.mutex);
    _switcher.status = SWITCHER_ABORTED;
    vmutex_release(&_switcher.mutex);
}
