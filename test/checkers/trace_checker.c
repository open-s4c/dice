/*
 * Copyright (C) 2025 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <assert.h>

#define LOG_PREFIX "TRACE_CHECKER: "
#ifndef LOG_LEVEL
    #define LOG_LEVEL INFO
#endif

#include "trace_checker.h"
#include <dice/chains/capture.h>
#include <dice/ensure.h>
#include <dice/events/self.h>
#include <dice/log.h>
#include <dice/module.h>
#include <dice/self.h>

#define MAX_NTHREADS  128
#define MAX_POSTPONED 1024

struct postponed {
    struct expected_event events[MAX_POSTPONED];
    size_t count;
    size_t checked;
};
static struct postponed _postponed;

struct thread_trace {
    struct expected_event *trace;
    struct expected_event *next;
};
static struct thread_trace _expected[MAX_NTHREADS];

void
register_expected_trace(thread_id tid, struct expected_event *trace)
{
    assert(tid > 0 && tid < MAX_NTHREADS);
    log_info("register expected trace thread=%lu", tid);
    _expected[tid] = (struct thread_trace){.trace = trace, .next = trace};
}

static void
_check_next(struct thread_trace *exp, chain_id chain, type_id type,
            thread_id id)
{
    struct expected_event *next = exp->next;
    if (!next->set) {
        log_warn("[%" PRIu64
                 "] trace ends too early:\n"
                 "\t\t      expected event %s:%s",
                 id, ps_chain_str(chain), ps_type_str(type));
        return;
    }
    if (!next->wild) {
        bool mismatch = next->chain != chain || next->type != type;
        if (mismatch && next->atleast > 0) {
            log_fatal("[%" PRIu64
                      "] event %s:%s\n"
                      "\t\t      mismatch %s:%s",
                      id, ps_chain_str(chain), ps_type_str(type),
                      ps_chain_str(next->chain), ps_type_str(next->type));
        } else if (mismatch) {
            exp->next++;
            log_warn("[%" PRIu64 "] event %s:%s mismatch (go to next)", id,
                     ps_chain_str(chain), ps_type_str(type));
            _check_next(exp, chain, type, id);
            return;
        }
        log_warn("[%" PRIu64 "] event %s:%s match", id, ps_chain_str(chain),
                 ps_type_str(type));
        if (next->atleast > 0)
            next->atleast--;
        if (next->atmost-- == 1)
            exp->next++;
    } else {
        if (next->chain != chain || next->type != type) {
            log_warn("[%" PRIu64
                     "] event %s:%s\n"
                     "\t\t      wild match %s:%s",
                     id, ps_chain_str(chain), ps_type_str(type),
                     ps_chain_str(next->chain), ps_type_str(next->type));
            ;
            return;
        }
        log_warn("[%" PRIu64 "] event %s:%s match suffix", id,
                 ps_chain_str(chain), ps_type_str(type));
        ensure(next->atleast == next->atmost);
        ensure(next->atleast == 1);
        exp->next++;
    }
}

static void
_postpone_event(chain_id chain, type_id type, struct metadata *md)
{
    thread_id id = self_id(md);
    log_warn("[%" PRIu64 "] postpone event %s:%s", id, ps_chain_str(chain),
             ps_type_str(type));
    struct postponed *pp = SELF_TLS(md, &_postponed);
    if (pp->count >= MAX_POSTPONED)
        log_fatal("[%" PRIu64 "] too many postponed events: %lu", id,
                  pp->count);
    pp->events[pp->count++] = EXPECTED_EVENT(chain, type);
}

static void
_check_trace(chain_id chain, type_id type, struct metadata *md)
{
    thread_id id = self_id(md);
    if (id >= MAX_NTHREADS)
        log_fatal("[%" PRIu64 "] thread id too large", id);

    struct thread_trace *exp = &_expected[id];
    ensure(exp);

    if (id == MAIN_THREAD) {
        if (exp->next == NULL) {
            _postpone_event(chain, type, md);
            return;
        }
        struct postponed *pp = SELF_TLS(md, &_postponed);
        while (pp->count > pp->checked) {
            struct expected_event *pe = &pp->events[pp->checked++];
            _check_next(exp, pe->chain, pe->type, id);
        }
        // fallthrough: continue checking current event
    }

    if (exp->next == NULL)
        log_fatal("[%" PRIu64 "] not yet registered: expected %d:%d", id, chain,
                  type);

    _check_next(exp, chain, type, id);
}

PS_SUBSCRIBE(CAPTURE_EVENT, ANY_EVENT, { _check_trace(chain, type, md); })
PS_SUBSCRIBE(CAPTURE_BEFORE, ANY_EVENT, { _check_trace(chain, type, md); })
PS_SUBSCRIBE(CAPTURE_AFTER, ANY_EVENT, { _check_trace(chain, type, md); })
