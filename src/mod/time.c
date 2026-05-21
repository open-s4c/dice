/*
 * Copyright (C) 2026 Huawei Technologies Co., Ltd.
 * SPDX-License-Identifier: 0BSD
 */
#include <dice/chains/intercept.h>
#include <dice/events/time.h>
#include <dice/interpose.h>
#include <dice/module.h>
#include <dice/pubsub.h>

INTERPOSE(char *, asctime, const struct tm *timeptr)
{
    struct asctime_event ev = {
        .pc = INTERPOSE_PC,
        .timeptr = timeptr,
        .ret = NULL,
        .func = REAL_FUNC(asctime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ASCTIME, &ev, &md);
    ev.ret = ev.func(ev.timeptr);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ASCTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(char*, asctime_r, const struct tm *tm, char buf[])
{
    struct asctime_r_event ev = {
        .pc = INTERPOSE_PC,
        .tm = tm,
        .buf = buf,
        .ret = NULL,
        .func = REAL_FUNC(asctime_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_ASCTIME_R, &ev, &md);
    ev.ret = ev.func(ev.tm, ev.buf);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_ASCTIME_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(clock_t, clock, void)
{
    struct clock_event ev = {
        .pc = INTERPOSE_PC,
        .ret = 0,
        .func = REAL_FUNC(clock)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOCK, &ev, &md);
    ev.ret = ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOCK, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, clock_getres, clockid_t clockid, struct timespec *res)
{
    struct clock_getres_event ev = {
        .pc = INTERPOSE_PC,
        .clockid = clockid,
        .res = res,
        .ret = 0,
        .func = REAL_FUNC(clock_getres)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOCK_GETRES, &ev, &md);
    ev.ret = ev.func(ev.clockid, ev.res);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOCK_GETRES, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, clock_gettime, clockid_t clockid, struct timespec *tp)
{
    struct clock_gettime_event ev = {
        .pc = INTERPOSE_PC,
        .clockid = clockid,
        .tp = tp,
        .ret = 0,
        .func = REAL_FUNC(clock_gettime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOCK_GETTIME, &ev, &md);
    ev.ret = ev.func(ev.clockid, ev.tp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOCK_GETTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, clock_settime, clockid_t clockid, const struct timespec *tp)
{
    struct clock_settime_event ev = {
        .pc = INTERPOSE_PC,
        .clockid = clockid,
        .tp = tp,
        .ret = 0,
        .func = REAL_FUNC(clock_settime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CLOCK_SETTIME, &ev, &md);
    ev.ret = ev.func(ev.clockid, ev.tp);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CLOCK_SETTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, ctime, const time_t *timep)
{
    struct ctime_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .ret = NULL,
        .func = REAL_FUNC(ctime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CTIME, &ev, &md);
    ev.ret = ev.func(ev.timep);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(char *, ctime_r, const time_t *timep, char buf[])
{
    struct ctime_r_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .ret = NULL,
        .func = REAL_FUNC(ctime_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_CTIME_R, &ev, &md);
    ev.ret = ev.func(ev.timep, buf);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_CTIME_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(double, difftime, time_t time1, time_t time0)
{
    struct difftime_event ev = {
        .pc = INTERPOSE_PC,
        .time1 = time1,
        .time0 = time0,
        .ret = 0.0,
        .func = REAL_FUNC(difftime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_DIFFTIME, &ev, &md);
    ev.ret = ev.func(ev.time1, ev.time0);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_DIFFTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(struct tm *, getdate, const char *string)
{
    struct getdate_event ev = {
        .pc = INTERPOSE_PC,
        .string = string,
        .ret = NULL,
        .func = REAL_FUNC(getdate)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETDATE, &ev, &md);
    ev.ret = ev.func(ev.string);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETDATE, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, getdate_r, const char *string, struct tm *res)
{
    struct getdate_r_event ev = {
        .pc = INTERPOSE_PC,
        .string = string,
        .res = res,
        .ret = 0,
        .func = REAL_FUNC(getdate_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETDATE_R, &ev, &md);
    ev.ret = ev.func(ev.string, ev.res);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETDATE_R, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(struct tm *, gmtime, const time_t *timep)
{
    struct gmtime_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .ret = NULL,
        .func = REAL_FUNC(gmtime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GMTIME, &ev, &md);
    ev.ret = ev.func(ev.timep);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GMTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(struct tm *, gmtime_r, const time_t *timep, struct tm *result)
{
    struct gmtime_r_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .result = result,
        .ret = NULL,
        .func = REAL_FUNC(gmtime_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GMTIME_R, &ev, &md);
    ev.ret = ev.func(ev.timep, ev.result);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GMTIME_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(struct tm *, localtime, const time_t *timep)
{
    struct localtime_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .ret = NULL,
        .func = REAL_FUNC(localtime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LOCALTIME, &ev, &md);
    ev.ret = ev.func(ev.timep);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LOCALTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(struct tm *, localtime_r, const time_t *timep, struct tm *result)
{
    struct localtime_r_event ev = {
        .pc = INTERPOSE_PC,
        .timep = timep,
        .result = result,
        .ret = NULL,
        .func = REAL_FUNC(localtime_r)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_LOCALTIME_R, &ev, &md);
    ev.ret = ev.func(ev.timep, ev.result);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_LOCALTIME_R, &ev, &md);
    return ev.ret;
}

INTERPOSE(time_t, mktime, struct tm *tm)
{
    struct mktime_event ev = {
        .pc = INTERPOSE_PC,
        .tm = tm,
        .ret = 0,
        .func = REAL_FUNC(mktime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_MKTIME, &ev, &md);
    ev.ret = ev.func(ev.tm);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_MKTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, nanosleep, const struct timespec *duration, struct timespec *rem)
{
    struct nanosleep_event ev = {
        .pc = INTERPOSE_PC,
        .duration = duration,
        .rem = rem,
        .ret = 0,
        .func = REAL_FUNC(nanosleep)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_NANOSLEEP, &ev, &md);
    ev.ret = ev.func(ev.duration, ev.rem);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_NANOSLEEP, &ev, &md);
    return ev.ret;
}

INTERPOSE(size_t, strftime, char *s, size_t max, const char *format, const struct tm *tm)
{
    struct strftime_event ev = {
        .pc = INTERPOSE_PC,
        .s = s,
        .max = max,
        .format = format,
        .tm = tm,
        .ret = 0,
        .func = REAL_FUNC(strftime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_STRFTIME, &ev, &md);
    ev.ret = ev.func(ev.s, ev.max, ev.format, ev.tm);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_STRFTIME, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(size_t, strftime_l, char *s, size_t max, const char *format, const struct tm *tm, locale_t locale)
{
    struct strftime_l_event ev = {
        .pc = INTERPOSE_PC,
        .s = s,
        .max = max,
        .format = format,
        .tm = tm,
        .locale = locale,
        .ret = 0,
        .func = REAL_FUNC(strftime_l)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_STRFTIME_L, &ev, &md);
    ev.ret = ev.func(ev.s, ev.max, ev.format, ev.tm, ev.locale);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_STRFTIME_L, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(char *, strptime, const char *s, const char *format, struct tm *tm)
{
    struct strptime_event ev = {
        .pc = INTERPOSE_PC,
        .s = s,
        .format = format,
        .tm = tm,
        .ret = NULL,
        .func = REAL_FUNC(strptime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_STRPTIME, &ev, &md);
    ev.ret = ev.func(ev.s, ev.format, ev.tm);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_STRPTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(time_t, time, time_t *tloc)
{
    struct time_event ev = {
        .pc = INTERPOSE_PC,
        .tloc = tloc,
        .ret = 0,
        .func = REAL_FUNC(time)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIME, &ev, &md);
    ev.ret = ev.func(ev.tloc);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIME, &ev, &md);
    return ev.ret;
}

#if !defined(__APPLE__)
INTERPOSE(int, timer_create, clockid_t clockid, struct sigevent *sevp, timer_t *timerid)
{
    struct timer_create_event ev = {
        .pc = INTERPOSE_PC,
        .clockid = clockid,
        .sevp = sevp,
        .timerid = timerid,
        .ret = 0,
        .func = REAL_FUNC(timer_create)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIMER_CREATE, &ev, &md);
    ev.ret = ev.func(ev.clockid, ev.sevp, ev.timerid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIMER_CREATE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, timer_delete, timer_t timerid)
{
    struct timer_delete_event ev = {
        .pc = INTERPOSE_PC,
        .timerid = timerid,
        .ret = 0,
        .func = REAL_FUNC(timer_delete)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIMER_DELETE, &ev, &md);
    ev.ret = ev.func(ev.timerid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIMER_DELETE, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, timer_gettime, timer_t timerid, struct itimerspec *curr_value)
{
    struct timer_gettime_event ev = {
        .pc = INTERPOSE_PC,
        .timerid = timerid,
        .curr_value = curr_value,
        .ret = 0,
        .func = REAL_FUNC(timer_gettime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIMER_GETTIME, &ev, &md);
    ev.ret = ev.func(ev.timerid, ev.curr_value);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIMER_GETTIME, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, timer_getoverrun, timer_t timerid)
{
    struct timer_getoverrun_event ev = {
        .pc = INTERPOSE_PC,
        .timerid = timerid,
        .ret = 0,
        .func = REAL_FUNC(timer_getoverrun)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIMER_GETOVERRUN, &ev, &md);
    ev.ret = ev.func(ev.timerid);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIMER_GETOVERRUN, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, timer_settime, timer_t timerid, int flags, const struct itimerspec *new_value, struct itimerspec *old_value)
{
    struct timer_settime_event ev = {
        .pc = INTERPOSE_PC,
        .timerid = timerid,
        .flags = flags,
        .new_value = new_value,
        .old_value = old_value,
        .ret = 0,
        .func = REAL_FUNC(timer_settime)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TIMER_SETTIME, &ev, &md);
    ev.ret = ev.func(ev.timerid, ev.flags, ev.new_value, ev.old_value);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TIMER_SETTIME, &ev, &md);
    return ev.ret;
}
#endif

INTERPOSE(void, tzset)
{
    struct tzset_event ev = {
        .pc = INTERPOSE_PC,
        .func = REAL_FUNC(tzset)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_TZSET, &ev, &md);
    ev.func();
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_TZSET, &ev, &md);
}

INTERPOSE(int, getitimer, __itimer_which_t which, struct itimerval *curr_value)
{
    struct getitimer_event ev = {
        .pc = INTERPOSE_PC,
        .which = which,
        .curr_value = curr_value,
        .ret = 0,
        .func = REAL_FUNC(getitimer)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETITIMER, &ev, &md);
    ev.ret = ev.func(ev.which, ev.curr_value);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETITIMER, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, setitimer, __itimer_which_t which, const struct itimerval *new_value, struct itimerval *old_value)
{
    struct setitimer_event ev = {
        .pc = INTERPOSE_PC,
        .which = which,
        .new_value = new_value,
        .old_value = old_value,
        .ret = 0,
        .func = REAL_FUNC(setitimer)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETITIMER, &ev, &md);
    ev.ret = ev.func(ev.which, ev.new_value, ev.old_value);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETITIMER, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, gettimeofday, struct timeval *tv, void *tz)
{
    struct gettimeofday_event ev = {
        .pc = INTERPOSE_PC,
        .tv = tv,
        .tz = tz,
        .ret = 0,
        .func = REAL_FUNC(gettimeofday)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_GETTIMEOFDAY, &ev, &md);
    ev.ret = ev.func(ev.tv, ev.tz);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_GETTIMEOFDAY, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, settimeofday, const struct timeval *tv, const struct timezone *tz)
{
    struct settimeofday_event ev = {
        .pc = INTERPOSE_PC,
        .tv = tv,
        .tz = tz,
        .ret = 0,
        .func = REAL_FUNC(settimeofday)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SETTIMEOFDAY, &ev, &md);
    ev.ret = ev.func(ev.tv, ev.tz);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SETTIMEOFDAY, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, select, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    struct select_event ev = {
        .pc = INTERPOSE_PC,
        .nfds = nfds,
        .readfds = readfds,
        .writefds = writefds,
        .exceptfds = exceptfds,
        .timeout = timeout,
        .ret = 0,
        .func = REAL_FUNC(select)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_SELECT, &ev, &md);
    ev.ret = ev.func(ev.nfds, ev.readfds, ev.writefds, ev.exceptfds, ev.timeout);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_SELECT, &ev, &md);
    return ev.ret;
}

INTERPOSE(int, utimes, const char *path, const struct timeval times[2])
{
    struct utimes_event ev = {
        .pc = INTERPOSE_PC,
        .path = path,
        .times = times,
        .ret = 0,
        .func = REAL_FUNC(utimes)
    };

    metadata_t md = {0};
    PS_PUBLISH(INTERCEPT_BEFORE, EVENT_UTIMES, &ev, &md);
    ev.ret = ev.func(ev.path, ev.times);
    PS_PUBLISH(INTERCEPT_AFTER, EVENT_UTIMES, &ev, &md);
    return ev.ret;
}

/* Advertise event type names for debugging messages */
PS_ADVERTISE_TYPE(EVENT_ASCTIME)
PS_ADVERTISE_TYPE(EVENT_ASCTIME_R)
PS_ADVERTISE_TYPE(EVENT_CLOCK)
PS_ADVERTISE_TYPE(EVENT_CLOCK_GETRES)
PS_ADVERTISE_TYPE(EVENT_CLOCK_GETTIME)
PS_ADVERTISE_TYPE(EVENT_CLOCK_SETTIME)
PS_ADVERTISE_TYPE(EVENT_CTIME)
PS_ADVERTISE_TYPE(EVENT_CTIME_R)
PS_ADVERTISE_TYPE(EVENT_DIFFTIME)
PS_ADVERTISE_TYPE(EVENT_GETDATE)
PS_ADVERTISE_TYPE(EVENT_GETDATE_R)
PS_ADVERTISE_TYPE(EVENT_GMTIME)
PS_ADVERTISE_TYPE(EVENT_GMTIME_R)
PS_ADVERTISE_TYPE(EVENT_LOCALTIME)
PS_ADVERTISE_TYPE(EVENT_LOCALTIME_R)
PS_ADVERTISE_TYPE(EVENT_MKTIME)
PS_ADVERTISE_TYPE(EVENT_NANOSLEEP)
PS_ADVERTISE_TYPE(EVENT_STRFTIME)
PS_ADVERTISE_TYPE(EVENT_STRFTIME_L)
PS_ADVERTISE_TYPE(EVENT_STRPTIME)
PS_ADVERTISE_TYPE(EVENT_TIME)
PS_ADVERTISE_TYPE(EVENT_TIMER_CREATE)
PS_ADVERTISE_TYPE(EVENT_TIMER_DELETE)
PS_ADVERTISE_TYPE(EVENT_TIMER_GETTIME)
PS_ADVERTISE_TYPE(EVENT_TIMER_GETOVERRUN)
PS_ADVERTISE_TYPE(EVENT_TIMER_SETTIME)
PS_ADVERTISE_TYPE(EVENT_TZSET)
PS_ADVERTISE_TYPE(EVENT_GETITIMER)
PS_ADVERTISE_TYPE(EVENT_SETITIMER)
PS_ADVERTISE_TYPE(EVENT_GETTIMEOFDAY)
PS_ADVERTISE_TYPE(EVENT_SETTIMEOFDAY)
PS_ADVERTISE_TYPE(EVENT_SELECT)
PS_ADVERTISE_TYPE(EVENT_UTIMES)

/* Mark module initialization (optional) */
DICE_MODULE_INIT()
