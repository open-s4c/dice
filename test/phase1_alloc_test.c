/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

#include <dice/ensure.h>
#include <dice/events/cpuset_alloc.h>
#include <dice/events/path_alloc.h>
#include <dice/phase1_alloc.h>

static char *(*phase1_strdup)(const char *)                    = strdup;
static char *(*phase1_strndup)(const char *, size_t)           = strndup;
static wchar_t *(*phase1_wcsdup)(const wchar_t *)              = wcsdup;
static char *(*phase1_getcwd)(char *, size_t)                  = getcwd;
static char *(*phase1_realpath)(const char *, char *)          = realpath;
static char *(*phase1_tempnam)(const char *, const char *)     = tempnam;
static int (*phase1_asprintf)(char **, const char *, ...)      = asprintf;
static int (*phase1_vasprintf)(char **, const char *, va_list) = vasprintf;

#ifdef HAVE_GET_CURRENT_DIR_NAME
static char *(*phase1_get_current_dir_name)(void) = get_current_dir_name;
#endif

#ifdef HAVE_SCHED_CPUALLOC
static cpu_set_t *(*phase1_sched_cpualloc)(size_t) = __sched_cpualloc;
#endif

static char *
call_vasprintf(const char *fmt, ...)
{
    char *buf = NULL;
    va_list ap;

    va_start(ap, fmt);
    ensure(phase1_vasprintf(&buf, fmt, ap) >= 0);
    va_end(ap);
    return buf;
}

int
main(void)
{
    char *saved_cwd = phase1_getcwd(NULL, 0);
    char tmpdir[]   = PHASE1_TMPDIR_TEMPLATE;

    ensure(saved_cwd != NULL);
    ensure(mkdtemp(tmpdir) != NULL);
    ensure(chdir(tmpdir) == 0);

    char *dup = phase1_strdup(PHASE1_STRDUP_ARG);
    ensure(dup != NULL);
    free(dup);

    char *ndup = phase1_strndup(PHASE1_STRNDUP_ARG, PHASE1_STRNDUP_N);
    ensure(ndup != NULL);
    free(ndup);

    wchar_t *wdup = phase1_wcsdup(PHASE1_WCSDUP_ARG);
    ensure(wdup != NULL);
    free(wdup);

#ifdef HAVE_GET_CURRENT_DIR_NAME
    char *dir_name = phase1_get_current_dir_name();
    ensure(dir_name != NULL);
    free(dir_name);
#endif

    char *cwd = phase1_getcwd(NULL, PHASE1_GETCWD_SIZE);
    ensure(cwd != NULL);
    free(cwd);

    char *resolved = phase1_realpath(PHASE1_REALPATH_ARG, NULL);
    ensure(resolved != NULL);
    free(resolved);

    char *tmp = phase1_tempnam(NULL, PHASE1_TEMPNAM_PFX);
    ensure(tmp != NULL);
    free(tmp);

    char *printed = NULL;
    ensure(phase1_asprintf(&printed, PHASE1_ASPRINTF_FMT,
                           PHASE1_ASPRINTF_ARG) >= 0);
    free(printed);

    char *vprinted = call_vasprintf(PHASE1_VASPRINTF_FMT, PHASE1_VASPRINTF_ARG);
    ensure(vprinted != NULL);
    free(vprinted);

#ifdef HAVE_SCHED_CPUALLOC
    cpu_set_t *set = phase1_sched_cpualloc(PHASE1_CPUSET_COUNT);
    ensure(set != NULL);
    CPU_FREE(set);
#endif

    ensure(chdir(saved_cwd) == 0);
    ensure(rmdir(tmpdir) == 0);
    free(saved_cwd);

    return 0;
}
