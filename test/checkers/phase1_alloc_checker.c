/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <string.h>
#include <wchar.h>

#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/cpuset_alloc.h>
#include <dice/events/path_alloc.h>
#include <dice/events/printf_alloc.h>
#include <dice/events/string_alloc.h>
#include <dice/module.h>
#include <dice/phase1_alloc.h>
#include <dice/pubsub.h>

static int saw_strdup;
static int saw_strndup;
#ifdef HAVE_WCSDUP
static int saw_wcsdup;
#endif
#ifdef HAVE_GET_CURRENT_DIR_NAME
static int saw_get_current_dir_name;
#endif
static int saw_getcwd;
static int saw_realpath;
#ifdef HAVE_TEMPNAM
static int saw_tempnam;
#endif
#ifdef HAVE_ASPRINTF
static int saw_asprintf;
#endif
#ifdef HAVE_VASPRINTF
static int saw_vasprintf;
#endif
#ifdef HAVE_SCHED_CPUALLOC
static int saw_sched_cpualloc;
#endif

static int
is_phase1_path(const char *path)
{
    return path != NULL && strstr(path, PHASE1_TMPDIR_PREFIX) != NULL;
}

static int
is_phase1_realpath(const char *path)
{
    const char *suffix = "/" PHASE1_REALPATH_ARG;
    size_t path_len    = strlen(path);
    size_t suffix_len  = strlen(suffix);

    if (!is_phase1_path(path) || path_len < suffix_len)
        return 0;
    return strcmp(path + path_len - suffix_len, suffix) == 0;
}

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_STRDUP, {
    struct strdup_event *ev = EVENT_PAYLOAD(ev);
    if (ev->s != NULL && strcmp(ev->s, PHASE1_STRDUP_ARG) == 0) {
        ensure(ev->ret != NULL);
        saw_strdup++;
    }
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_STRNDUP, {
    struct strndup_event *ev = EVENT_PAYLOAD(ev);
    if (ev->s != NULL && strcmp(ev->s, PHASE1_STRNDUP_ARG) == 0 &&
        ev->n == PHASE1_STRNDUP_N) {
        ensure(ev->ret != NULL);
        saw_strndup++;
    }
})

#ifdef HAVE_WCSDUP
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_WCSDUP, {
    struct wcsdup_event *ev = EVENT_PAYLOAD(ev);
    if (ev->s != NULL && wcscmp(ev->s, PHASE1_WCSDUP_ARG) == 0) {
        ensure(ev->ret != NULL);
        saw_wcsdup++;
    }
})
#endif

#ifdef HAVE_GET_CURRENT_DIR_NAME
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_GET_CURRENT_DIR_NAME, {
    struct get_current_dir_name_event *ev = EVENT_PAYLOAD(ev);
    if (is_phase1_path(ev->ret)) {
        ensure(ev->ret != NULL);
        saw_get_current_dir_name++;
    }
})
#endif

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_GETCWD, {
    struct getcwd_event *ev = EVENT_PAYLOAD(ev);
    if (ev->buf == NULL && ev->size == PHASE1_GETCWD_SIZE) {
        ensure(ev->ret != NULL);
        ensure(is_phase1_path(ev->ret));
        saw_getcwd++;
    }
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_REALPATH, {
    struct realpath_event *ev = EVENT_PAYLOAD(ev);
    if (ev->path != NULL && is_phase1_realpath(ev->path) &&
        ev->resolved_path == NULL && ev->ret != NULL &&
        is_phase1_realpath(ev->ret)) {
        saw_realpath++;
    }
})

#ifdef HAVE_TEMPNAM
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_TEMPNAM, {
    struct tempnam_event *ev = EVENT_PAYLOAD(ev);
    if (ev->dir == NULL && ev->pfx != NULL &&
        strcmp(ev->pfx, PHASE1_TEMPNAM_PFX) == 0) {
        ensure(ev->ret != NULL);
        saw_tempnam++;
    }
})
#endif

#ifdef HAVE_ASPRINTF
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_ASPRINTF, {
    struct asprintf_event *ev = EVENT_PAYLOAD(ev);
    if (ev->fmt != NULL && strcmp(ev->fmt, PHASE1_ASPRINTF_FMT) == 0) {
        ensure(ev->ret >= 0);
        ensure(ev->strp != NULL && *ev->strp != NULL);
        saw_asprintf++;
    }
})
#endif

#ifdef HAVE_VASPRINTF
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_VASPRINTF, {
    struct vasprintf_event *ev = EVENT_PAYLOAD(ev);
    if (ev->fmt != NULL && strcmp(ev->fmt, PHASE1_VASPRINTF_FMT) == 0) {
        ensure(ev->ret >= 0);
        ensure(ev->strp != NULL && *ev->strp != NULL);
        saw_vasprintf++;
    }
})
#endif

#ifdef HAVE_SCHED_CPUALLOC
PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_SCHED_CPUALLOC, {
    struct sched_cpualloc_event *ev = EVENT_PAYLOAD(ev);
    if (ev->count == PHASE1_CPUSET_COUNT) {
        ensure(ev->ret != NULL);
        saw_sched_cpualloc++;
    }
})
#endif

DICE_MODULE_FINI({
    ensure(saw_strdup == 1);
    ensure(saw_strndup == 1);
#ifdef HAVE_WCSDUP
    ensure(saw_wcsdup == 1);
#endif
#ifdef HAVE_GET_CURRENT_DIR_NAME
    ensure(saw_get_current_dir_name == 1);
#endif
    ensure(saw_getcwd == 1);
    ensure(saw_realpath == 1);
#ifdef HAVE_TEMPNAM
    ensure(saw_tempnam == 1);
#endif
#ifdef HAVE_ASPRINTF
    ensure(saw_asprintf == 1);
#endif
#ifdef HAVE_VASPRINTF
    ensure(saw_vasprintf == 1);
#endif
#ifdef HAVE_SCHED_CPUALLOC
    ensure(saw_sched_cpualloc == 1);
#endif
})
