/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: 0BSD
 */

#include <stdbool.h>
#include <string.h>

#include <dice/ensure.h>
#include <dice/interpose.h>
#include <dice/log.h>

#define MAX_EXP 16
static char *strings[MAX_EXP] = {0};
static char **head            = &strings[0];
static char **tail            = &strings[0];

INTERPOSE(ssize_t, write, int fd, const void *buf, size_t count)
{
    static int nest = 0;
    if (nest == 1) {
        return REAL(write, fd, buf, count);
    }
    ensure(nest == 0);
    nest++;
    if (*tail == NULL) {
        log_printf("ERROR: *tail is NULL\n");
        log_printf("tail == %p\n", tail);
        log_printf("strings == %p\n", &strings[0]);
        abort();
    }
    ensure(tail < head);
    if (strncmp((char *)buf, *tail, count) != 0) {
        log_printf("ERROR: unexpected entry\n");
        log_printf("exp: %s\n", *tail);
        log_printf("buf: %s\n", (char *)buf);
        abort();
    }
    tail++;
    nest--;
    return count;
}

static void
expect(char *e)
{
    ensure(head < strings + MAX_EXP);
    *head = e;
    head++;
}

static bool
empty(void)
{
    return head == tail;
}

int
main()
{
    // this should always work
    expect("print");
    log_printf("print");
    ensure(empty());

    // this should always work, but we remove the abort to about actually
    // aborting
    expect("dice: ");
    expect("fatal");
    expect("\n");
#define abort()
    log_fatal("fatal");
#undef abort
    ensure(empty());

#if LOG_LEVEL_ >= LOG_LEVEL_INFO
    expect("dice: ");
    expect("info");
    expect("\n");
    log_info("info");
    ensure(empty());
#endif

#if LOG_LEVEL_ >= LOG_LEVEL_DEBUG
    expect("dice: ");
    expect("debug");
    expect("\n");
    log_debug("debug");
    ensure(empty());
#endif

    return 0;
}
