/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <stdlib.h>

/* Ensure BINGO_XTOR_PRIO is set to nothing to initialize this as the last
 * module in the main library */
#ifdef BINGO_XTOR_PRIO
    #undef BINGO_XTRO_PRIO
#endif
#define BINGO_XTOR_PRIO 999
#include <bingo/compiler.h>
#include <bingo/log.h>
#include <bingo/module.h>
#include <bingo/pubsub.h>

/* ensure this is the last to be initialized by setting no priority for
 * constructor
 * By using BINGO_HIDE here, inside the core .so this is the version that will
 * be used */

static bool _initd;
static void
_init_check(void)
{
    if (_initd) {
        perror("subscription without priority in main .so");
        exit(EXIT_FAILURE);
    }
}

struct callback {
    chain_id chain;
    type_id type;
    const char *fname;
};

struct publication {
    chain_id chain;
    type_id type;
    const char *fname;
};


BINGO_WEAK void
ps_register_callback(chain_id chain, type_id type, const char *fname)
{
    _init_check();
    printf("registering callback %d:%d:%s\n", chain, type, fname);
}

// BINGO_WEE void
// ps_register_broker(chain_id chain, type_id type, const char *fname)
//{
//     _init_check();
//     printf("registering broken %d:%d:%s\n", chain, type, fname);
// }

BINGO_MODULE_INIT({ _initd = true; })
