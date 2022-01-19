/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <time.h>
#include <export.h>

/**
 * mktime - converts date to seconds
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
