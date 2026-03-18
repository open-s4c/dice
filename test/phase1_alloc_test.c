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
#include <dice/events/printf_alloc.h>
#include <dice/events/string_alloc.h>
#include <dice/phase1_alloc.h>

#ifdef HAVE_VASPRINTF
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
#endif

int
main(void)
{
    char *saved_cwd = getcwd(NULL, 0);
    char tmpdir[]   = PHASE1_TMPDIR_TEMPLATE;
    FILE *realpath_file;

    ensure(saved_cwd != NULL);
    ensure(mkdtemp(tmpdir) != NULL);
    ensure(chdir(tmpdir) == 0);
    realpath_file = fopen(PHASE1_REALPATH_ARG, "w");
    ensure(realpath_file != NULL);
    ensure(fclose(realpath_file) == 0);

    char *dup = strdup(PHASE1_STRDUP_ARG);
    ensure(dup != NULL);
    free(dup);

    char *ndup = strndup(PHASE1_STRNDUP_ARG, PHASE1_STRNDUP_N);
    ensure(ndup != NULL);
    free(ndup);

#ifdef HAVE_WCSDUP
    wchar_t *wdup = wcsdup(PHASE1_WCSDUP_ARG);
    ensure(wdup != NULL);
    free(wdup);
#endif

#ifdef HAVE_GET_CURRENT_DIR_NAME
    char *dir_name = get_current_dir_name();
    ensure(dir_name != NULL);
    free(dir_name);
#endif

    char *cwd = getcwd(NULL, PHASE1_GETCWD_SIZE);
    ensure(cwd != NULL);
    free(cwd);

    char *resolved = realpath(PHASE1_REALPATH_ARG, NULL);
    ensure(resolved != NULL);
    free(resolved);

#ifdef HAVE_TEMPNAM
    char *tmp = tempnam(NULL, PHASE1_TEMPNAM_PFX);
    ensure(tmp != NULL);
    free(tmp);
#endif

#ifdef HAVE_ASPRINTF
    char *printed = NULL;
    ensure(asprintf(&printed, PHASE1_ASPRINTF_FMT, PHASE1_ASPRINTF_ARG) >= 0);
    free(printed);
#endif

#ifdef HAVE_VASPRINTF
    char *vprinted = call_vasprintf(PHASE1_VASPRINTF_FMT, PHASE1_VASPRINTF_ARG);
    ensure(vprinted != NULL);
    free(vprinted);
#endif

#ifdef HAVE_SCHED_CPUALLOC
    cpu_set_t *set = __sched_cpualloc(PHASE1_CPUSET_COUNT);
    ensure(set != NULL);
    CPU_FREE(set);
#endif

    ensure(unlink(PHASE1_REALPATH_ARG) == 0);
    ensure(chdir(saved_cwd) == 0);
    ensure(rmdir(tmpdir) == 0);
    free(saved_cwd);

    return 0;
}
