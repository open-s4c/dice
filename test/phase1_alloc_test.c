/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <sched.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include <dice/ensure.h>

enum { ASPRINTF_ARG = 7, VASPRINTF_ARG = 9 };

static char *
call_vasprintf(const char *fmt, ...)
{
    char *buf = NULL;
    va_list ap;

    va_start(ap, fmt);
    ensure(vasprintf(&buf, fmt, ap) >= 0);
    va_end(ap);
    return buf;
}

int
main(void)
{
    char *dup = strdup("alpha");
    ensure(dup != NULL);
    free(dup);

    char *ndup = strndup("bravo", 3);
    ensure(ndup != NULL);
    free(ndup);

    wchar_t *wdup = wcsdup(L"charlie");
    ensure(wdup != NULL);
    free(wdup);

    char *dir_name = get_current_dir_name();
    ensure(dir_name != NULL);
    free(dir_name);

    char *cwd = getcwd(NULL, 0);
    ensure(cwd != NULL);
    free(cwd);

    char *resolved = realpath(".", NULL);
    ensure(resolved != NULL);
    free(resolved);

    char *tmp = tempnam(NULL, "ct");
    ensure(tmp != NULL);
    free(tmp);

    char *printed = NULL;
    ensure(asprintf(&printed, "%s-%d", "delta", ASPRINTF_ARG) >= 0);
    free(printed);

    char *vprinted = call_vasprintf("%s:%d", "echo", VASPRINTF_ARG);
    ensure(vprinted != NULL);
    free(vprinted);

    cpu_set_t *set = CPU_ALLOC(65);
    ensure(set != NULL);
    CPU_FREE(set);

    return 0;
}
