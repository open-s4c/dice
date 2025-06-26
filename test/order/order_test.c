/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include "defs.h"
int
main(int argc, char *argv[])
{
    if (argc != 2) {
        log_printf("usage: %s <subscribers>\n", argv[0]);
        exit(1);
    }
    int count = atoi(argv[1]);
    int final = count + 200;

    struct event ev = {0};
    publish(&ev);

    ensure(ev.position == final);
    ensure(ev.count == count);
    return 0;
}
