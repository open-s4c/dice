/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>

#include <dice/chains/sequence.h>
#include <dice/events/memaccess.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/pubsub.h>
#include <dice/self.h>
#include <dice/switcher.h>
#include <dice/thread_id.h>

static bool _started = false;

static enum ps_err
_sequence(chain_id chain, type_id type, void *event, struct plan *plan)
{
    if (self_retired(plan->self))
        return PS_STOP_CHAIN;
    if (!*SELF_TLS(plan->self, &_started))
        return PS_STOP_CHAIN;
    PS_PUBLISH(chain, type, event, &plan->_);
    if (plan->wake)
        switcher_wake(plan->next, 0);
    if (plan->yield)
        switcher_yield(self_id(plan->self), true);
    return PS_STOP_CHAIN;
}

#define DECL_PLAN                                                              \
    struct plan plan = {0};                                                    \
    plan.self        = md;                                                     \
    plan.me          = self_id(md);                                            \
    plan.chain       = chain;                                                  \
    plan.type        = type;                                                   \
    plan.next        = ANY_THREAD

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_TYPE, {
    DECL_PLAN;

    switch (type) {
        case EVENT_SELF_INIT: {
            // create started flag and initialize it with false (ie, 0)
            bool *started = SELF_TLS(md, &_started);
            // Dont sequence thread between SELF_INIT and THREAD_START, except
            // for main thread, which has no THREAD_START
            assert(!*started);
            if (plan.me == MAIN_THREAD) {
                plan.wake  = true;
                plan.yield = true;
                *started   = true;
            }
        } break;
        case EVENT_THREAD_START:
            // starting at this event, schedule thread
            assert(plan.me != MAIN_THREAD);
            *SELF_TLS(md, &_started) = true;
            plan.wake                = true;
            plan.yield               = true;
            break;
        case EVENT_THREAD_EXIT:
            // stop sequencing thread after this point until SELF_FINI
            plan.wake  = true;
            plan.yield = false;
            break;
        default:
            plan.wake  = true;
            plan.yield = true;
            break;
    }
    return _sequence(SEQUENCE_EVENT, type, event, &plan);
})

static bool
_is_memaccess(type_id type)
{
    return type >= EVENT_MA_READ && type <= EVENT_MA_FENCE;
}

PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_TYPE, {
    DECL_PLAN;
    plan.wake  = type != EVENT_THREAD_CREATE;
    plan.yield = _is_memaccess(type);
    return _sequence(SEQUENCE_BEFORE, type, event, &plan);
})

PS_SUBSCRIBE(CAPTURE_AFTER, ANY_TYPE, {
    DECL_PLAN;
    plan.wake  = false;
    plan.yield = !_is_memaccess(type);
    return _sequence(SEQUENCE_AFTER, type, event, &plan);
})

DICE_MODULE_INIT()
