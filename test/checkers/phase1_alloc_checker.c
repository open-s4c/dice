/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */

#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/cpuset_alloc.h>
#include <dice/events/path_alloc.h>
#include <dice/events/printf_alloc.h>
#include <dice/events/string_alloc.h>
#include <dice/module.h>
#include <dice/pubsub.h>

static int saw_strdup;
static int saw_strndup;
static int saw_wcsdup;
static int saw_get_current_dir_name;
static int saw_getcwd;
static int saw_realpath;
static int saw_tempnam;
static int saw_asprintf;
static int saw_vasprintf;
static int saw_sched_cpualloc;

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_STRDUP, {
    struct strdup_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_strdup++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_STRNDUP, {
    struct strndup_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_strndup++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_WCSDUP, {
    struct wcsdup_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_wcsdup++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_GET_CURRENT_DIR_NAME, {
    struct get_current_dir_name_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_get_current_dir_name++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_GETCWD, {
    struct getcwd_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_getcwd++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_REALPATH, {
    struct realpath_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_realpath++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_TEMPNAM, {
    struct tempnam_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_tempnam++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_ASPRINTF, {
    struct asprintf_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret >= 0);
    ensure(ev->strp != NULL && *ev->strp != NULL);
    saw_asprintf++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_VASPRINTF, {
    struct vasprintf_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret >= 0);
    ensure(ev->strp != NULL && *ev->strp != NULL);
    saw_vasprintf++;
})

PS_SUBSCRIBE(CAPTURE_AFTER, EVENT_SCHED_CPUALLOC, {
    struct sched_cpualloc_event *ev = EVENT_PAYLOAD(ev);
    ensure(ev->ret != NULL);
    saw_sched_cpualloc++;
})

DICE_MODULE_FINI({
    ensure(saw_strdup == 1);
    ensure(saw_strndup == 1);
    ensure(saw_wcsdup == 1);
    ensure(saw_get_current_dir_name == 1);
    ensure(saw_getcwd == 1);
    ensure(saw_realpath == 1);
    ensure(saw_tempnam == 1);
    ensure(saw_asprintf == 1);
    ensure(saw_vasprintf == 1);
    ensure(saw_sched_cpualloc == 1);
})
