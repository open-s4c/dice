/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/pubsub.h>

/* chain dispatcher prototypes */
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_0_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_0_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_1_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_1_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_2_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_2_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_3_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_3_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_4_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_4_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_5_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_5_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE enum ps_err
ps_dispatch_6_(const chain_id chain, const type_id type, void *event,
                 metadata_t *md)
{
    (void)chain;
    (void)type;
    (void)event;
    (void)md;
    return PS_HANDLER_OFF;
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_chain_6_on_(void)
{
    return false;
}

DICE_HIDE bool
ps_dispatch_chain_on_(const chain_id chain)
{
    switch (chain) {
        case 0:
            return ps_dispatch_chain_0_on_();
        case 1:
            return ps_dispatch_chain_1_on_();
        case 2:
            return ps_dispatch_chain_2_on_();
        case 3:
            return ps_dispatch_chain_3_on_();
        case 4:
            return ps_dispatch_chain_4_on_();
        case 5:
            return ps_dispatch_chain_5_on_();
        case 6:
            return ps_dispatch_chain_6_on_();
        default:
            return false;
    }
}

/* main dispatcher */
DICE_HIDE enum ps_err
ps_dispatch_(const chain_id chain, const type_id type, void *event,
             metadata_t *md)
{
    switch (chain) {
        default:
            return PS_HANDLER_OFF;
        case 0:
            return ps_dispatch_0_(chain, type, event, md);
        case 1:
            return ps_dispatch_1_(chain, type, event, md);
        case 2:
            return ps_dispatch_2_(chain, type, event, md);
        case 3:
            return ps_dispatch_3_(chain, type, event, md);
        case 4:
            return ps_dispatch_4_(chain, type, event, md);
        case 5:
            return ps_dispatch_5_(chain, type, event, md);
        case 6:
            return ps_dispatch_6_(chain, type, event, md);
    }
}

DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_0_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_1_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_2_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_3_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_4_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_5_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_6_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_7_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_8_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_9_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_10_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_11_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_12_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_13_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_14_on_(void)
{
    return false;
}
DICE_WEAK DICE_HIDE bool
ps_dispatch_slot_15_on_(void)
{
    return false;
}

DICE_HIDE int
ps_dispatch_max(void)
{
    int max = -1;
    if (ps_dispatch_slot_0_on_())
        max = max < 0 ? 0 : max;
    if (ps_dispatch_slot_1_on_())
        max = max < 1 ? 1 : max;
    if (ps_dispatch_slot_2_on_())
        max = max < 2 ? 2 : max;
    if (ps_dispatch_slot_3_on_())
        max = max < 3 ? 3 : max;
    if (ps_dispatch_slot_4_on_())
        max = max < 4 ? 4 : max;
    if (ps_dispatch_slot_5_on_())
        max = max < 5 ? 5 : max;
    if (ps_dispatch_slot_6_on_())
        max = max < 6 ? 6 : max;
    if (ps_dispatch_slot_7_on_())
        max = max < 7 ? 7 : max;
    if (ps_dispatch_slot_8_on_())
        max = max < 8 ? 8 : max;
    if (ps_dispatch_slot_9_on_())
        max = max < 9 ? 9 : max;
    if (ps_dispatch_slot_10_on_())
        max = max < 10 ? 10 : max;
    if (ps_dispatch_slot_11_on_())
        max = max < 11 ? 11 : max;
    if (ps_dispatch_slot_12_on_())
        max = max < 12 ? 12 : max;
    if (ps_dispatch_slot_13_on_())
        max = max < 13 ? 13 : max;
    if (ps_dispatch_slot_14_on_())
        max = max < 14 ? 14 : max;
    if (ps_dispatch_slot_15_on_())
        max = max < 15 ? 15 : max;

    return max;
}
