/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/events/memaccess.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/self.h>
#include <dice/switcher.h>

DICE_WEAK thread_id
pick_next_thread(chain_id chain, type_id type, void *event, metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return ANY_THREAD;
}

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_TYPE, {
    if (self_retired(md))
        return PS_STOP_CHAIN;

    switch (type) {
        case EVENT_THREAD_EXIT:
            switcher_wake(pick_next_thread(chain, type, event, md), 0);
            // stop sequencing thread after this point (self->retired == true)
            break;
        case EVENT_SELF_INIT:
            // threads call this only ONCE (except the main thread).
            if (self_id(md) == MAIN_THREAD)
                break;
            switcher_yield(self_id(md), true);
            break;
        default:
            switcher_wake(pick_next_thread(chain, type, event, md), 0);
            switcher_yield(self_id(md), true);
            break;
    }
    return PS_STOP_CHAIN;
})

PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_TYPE, {
    if (self_retired(md))
        return PS_STOP_CHAIN;

    switcher_wake(pick_next_thread(chain, type, event, md), 0);
    return PS_STOP_CHAIN;
})

PS_SUBSCRIBE(CAPTURE_AFTER, ANY_TYPE, {
    if (self_retired(md))
        return PS_STOP_CHAIN;

    switcher_yield(self_id(md), true);
    return PS_STOP_CHAIN;
})

DICE_MODULE_INIT()
