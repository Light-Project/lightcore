/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <time.h>
#include <export.h>

/**
 * mktime - converts date to seconds.
 * Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 */
time_t mktime(unsigned int year, unsigned int mon, unsigned int day,
              unsigned int hour, unsigned int min, unsigned int sec)
{
    time_t time;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int)(mon -= 2)) {
        mon += 12;
        year -= 1;
    }

    time = year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day;
    time = time + (year * 365 - 719499);
    time = (time * 24) + hour;  /* now it's hours */
    time = (time * 60) + min;   /* now it's minutes */
    time = (time * 60) + sec;   /* now it's seconds */

    return time;
}
EXPORT_SYMBOL(mktime);

struct timespec ns_to_timespec(const int64_t nsec)
{
    struct timespec ts;

    ts.tv_nsec = nsec % NSEC_PER_SEC;
    ts.tv_sec = nsec / NSEC_PER_SEC;

    return ts;
}
EXPORT_SYMBOL(ns_to_timespec);

int64_t timespec_to_ns(struct timespec *ts)
{
    if (ts->tv_sec >= KTIME_SEC_MAX)
        return KTIME_MAX;

    if (ts->tv_sec <= KTIME_SEC_MIN)
        return KTIME_MIN;

    return (ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}
EXPORT_SYMBOL(timespec_to_ns);

bool timespec_valid(struct timespec *ts)
{
    if (ts->tv_nsec >= NSEC_PER_SEC)
        return true;
    if (ts->tv_nsec < 0)
        return true;
    return false;
}
EXPORT_SYMBOL(timespec_valid);

void timespec_normalized(struct timespec *ts)
{
    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
    while (ts->tv_nsec < 0) {
        ts->tv_nsec += NSEC_PER_SEC;
        ts->tv_sec--;
    }
}
EXPORT_SYMBOL(timespec_normalized);

void timespec_set_normalized(struct timespec *ts, time_t sec, int64_t nsec)
{
    while (nsec >= NSEC_PER_SEC) {
        asm ("" : "+rm"(nsec));
        nsec -= NSEC_PER_SEC;
        sec++;
    }
    while (nsec < 0) {
        asm ("" : "+rm"(nsec));
        nsec += NSEC_PER_SEC;
        sec--;
    }
    ts->tv_sec = sec;
    ts->tv_nsec = nsec;
}
EXPORT_SYMBOL(timespec_set_normalized);

struct timespec timespec_add(struct timespec *lts, struct timespec *rts)
{
    struct timespec acc;
    acc.tv_nsec = lts->tv_nsec + rts->tv_nsec;
    acc.tv_sec = lts->tv_sec + rts->tv_sec;
    timespec_normalized(&acc);
    return acc;
}
EXPORT_SYMBOL(timespec_add);

struct timespec timespec_sub(struct timespec *lts, struct timespec *rts)
{
    struct timespec acc;
    acc.tv_nsec = lts->tv_nsec - rts->tv_nsec;
    acc.tv_sec = lts->tv_sec - rts->tv_sec;
    timespec_normalized(&acc);
    return acc;
}
EXPORT_SYMBOL(timespec_sub);
