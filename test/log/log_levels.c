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

REAL_DECL(ssize_t, write, int, const void *, size_t);
ssize_t
write(int fd, const void *buf, size_t count)
{
    static int nest = 0;
    if (nest == 1) {
        return REAL(write, fd, buf, count);
    }
    ensure(nest == 0);
    nest++;
    if (*tail == NULL) {
        log_abort(
            "ERROR: *tail is NULL\n"
            "tail == %p\n"
            "strings == %p\n",
            tail, &strings[0]);
    }
    ensure(tail < head);
    if (strncmp((char *)buf, *tail, count) != 0) {
        log_abort(
            "ERROR: unexpected entry\n"
            "exp: %s\n"
            "buf: %s\n",
            *tail, (char *)buf);
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

    // this should always work, we temporarily remove exit
    expect("dice: ");
    expect("fatal");
    expect("\n");
#define exit(...)
    log_fatal("fatal");
#undef exit
    ensure(empty());

    // this should always work, we temporarily remove abort
    expect("dice: ");
    expect("abort");
    expect("\n");
#define abort()
    log_abort("abort");
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
