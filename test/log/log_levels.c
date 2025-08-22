/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <dice/interpose.h>
#include <dice/log.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_EXP 16
static char *strings[MAX_EXP] = {0};
static char **head = &strings[0];
static char **tail = &strings[0];

INTERPOSE(ssize_t, write, int fd, const void *buf, size_t count) {
    static int nest = 0;
    if (nest == 1) {
        return REAL(write, fd, buf, count);
    }
    assert(nest == 0);
    nest++;
    assert(*tail);
    assert(tail < head);
    if (strncmp((char*) buf, *tail, count) != 0) {
        log_printf("exp: %s\n", *tail);
        log_printf("buf: %s\n", (char*)buf);
        abort();
    }
    tail++;
    nest--;
    return count;
}

static void expect(char *e) {
    assert(head < strings + MAX_EXP);
    *head = e;
    head++;
}
static bool empty(void) {
    return head == tail;
}


int
main()
{
    // this should always work
    expect("print");
    log_printf("print");
    assert(empty());

    // this should always work, but we remove the abort to about actually
    // aborting
    printf("level >= fatal\n");
    expect("dice: ");
    expect("fatal");
    expect("\n");
#define abort()
    log_fatal("fatal");
#undef abort
    assert(empty());

#if LOG_LEVEL_ >= LOG_LEVEL_INFO
    printf("level >= info\n");
    expect("dice: ");
    expect("info");
    expect("\n");
    log_info("info");
    assert(empty());
#endif

#if LOG_LEVEL_ >= LOG_LEVEL_DEBUG
    printf("level >= debug \n");
    expect("dice: ");
    expect("debug");
    expect("\n");
    log_debug("debug");
    assert(empty());
#endif


    return 0;
}
