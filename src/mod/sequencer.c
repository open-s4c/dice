/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/sequence.h>
#include <dice/events/memaccess.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/self.h>
#include <dice/switcher.h>

static enum ps_err
_sequence(chain_id chain, type_id type, void *event, struct plan *plan)
{
    if (self_retired(plan->self))
        return PS_STOP_CHAIN;
    PS_PUBLISH(chain, type, event, &plan->_);
    if (plan->wake)
        switcher_wake(plan->next, 0);
    if (plan->yield)
        switcher_yield(self_id(plan->self), true);
    return PS_STOP_CHAIN;
}


PS_SUBSCRIBE(CAPTURE_EVENT, ANY_TYPE, {
    struct plan plan;
    plan._    = (struct metadata){0};
    plan.self = md;
    plan.from = chain;
    plan.type = type;
    plan.next = ANY_THREAD;

    switch (type) {
        case EVENT_THREAD_EXIT:
            plan.wake  = true;
            plan.yield = false;
            // stop sequencing thread after this point until SELF_FINI
            // TODO: this must be reviewed
            break;
        case EVENT_SELF_INIT:
            // threads call this only ONCE except the main thread
            plan.wake  = false;
            plan.yield = self_id(md) != MAIN_THREAD;
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
    struct plan plan;
    plan._     = (struct metadata){0};
    plan.self  = md;
    plan.from  = chain;
    plan.type  = type;
    plan.next  = ANY_THREAD;
    plan.wake  = true;
    plan.yield = _is_memaccess(type);
    return _sequence(SEQUENCE_BEFORE, type, event, &plan);
})

PS_SUBSCRIBE(CAPTURE_AFTER, ANY_TYPE, {
    struct plan plan;
    plan._     = (struct metadata){0};
    plan.self  = md;
    plan.from  = chain;
    plan.type  = type;
    plan.next  = ANY_THREAD;
    plan.wake  = false;
    plan.yield = !_is_memaccess(type);
    return _sequence(SEQUENCE_AFTER, type, event, &plan);
})

DICE_MODULE_INIT()
