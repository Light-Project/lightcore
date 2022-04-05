/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <librtc.h>
#include <export.h>

const char *rtc_month_name[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec",
};
EXPORT_SYMBOL(rtc_month_name);

const char *rtc_wday_name[] = {
    "mon", "tue", "wed", "thur", "fri", "sat", "sun"
};
EXPORT_SYMBOL(rtc_wday_name);

static const unsigned int rtc_days_in_month[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const unsigned int rtc_year_to_day[2][13] = {
    /* Normal years */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

bool rtc_leap_year(unsigned int year)
{
    return (!(year % 4) && (year % 100)) || !(year % 400);
}
EXPORT_SYMBOL(rtc_leap_year);

/**
 * rtc_month_days - number of days in the month
 */
int rtc_month_days(unsigned int year, unsigned int month)
{
    return rtc_days_in_month[month] + (rtc_leap_year(year) && month == 1);
}
EXPORT_SYMBOL(rtc_month_days);

/**
 * rtc_year_days - number of days since January 1
 */
int rtc_year_days(unsigned int year, unsigned int month, unsigned int day)
{
    return rtc_year_to_day[rtc_leap_year(year)][month] + day - 1;
}
EXPORT_SYMBOL(rtc_year_days);

/**
 * rtc_time_to_tm - converts time_t to rtc_time
 * @time: number of seconds since 01-01-1970 00:00:00
 * @tm: pointer to the struct rtc_time
 */
void rtc_time_to_tm(time_t time, struct rtc_time *tm)
{
    uint32_t u32tmp, udays, century, day_of_century;
    uint32_t year_of_century, year, day_of_year, month, day;
    uint64_t u64tmp;
    unsigned int secs;
    int days;
    bool is_Jan_or_Feb, is_leap_year;

    /* time must be positive */
    secs            = time % 86400;
    days            = time / 86400;

    /* day of the week, 1970-01-01 was a Thursday */
    tm->tm_wday     = (days + 4) % 7;

    /*
     * The following algorithm is, basically, Proposition 6.3 of Neri
     * and Schneider [1]. In a few words: it works on the computational
     * (fictitious) calendar where the year starts in March, month = 2
     * (*), and finishes in February, month = 13. This calendar is
     * mathematically convenient because the day of the year does not
     * depend on whether the year is leap or not. For instance:
     *
     * March 1st  0-th day of the year;
     * ...
     * April 1st  31-st day of the year;
     * ...
     * January 1st  306-th day of the year; (Important!)
     * ...
     * February 28th 364-th day of the year;
     * February 29th 365-th day of the year (if it exists).
     *
     * After having worked out the date in the computational calendar
     * (using just arithmetics) it's easy to convert it to the
     * corresponding date in the Gregorian calendar.
     *
     * [1] "Euclidean Affine Functions and Applications to Calendar
     * Algorithms". https://arxiv.org/abs/2102.06959
     *
     * (*) The numbering of months follows rtc_time more closely and
     * thus, is slightly different from [1].
     */

    udays           = ((uint32_t) days) + 719468;

    u32tmp          = 4 * udays + 3;
    century         = u32tmp / 146097;
    day_of_century  = u32tmp % 146097 / 4;

    u32tmp          = 4 * day_of_century + 3;
    u64tmp          = 2939745ULL * u32tmp;
    year_of_century = upper_32_bits(u64tmp);
    day_of_year     = lower_32_bits(u64tmp) / 2939745 / 4;

    year            = 100 * century + year_of_century;
    is_leap_year    = year_of_century != 0 ?
                      year_of_century % 4 == 0 :
                      century % 4 == 0;

    u32tmp          = 2141 * day_of_year + 132377;
    month           = u32tmp >> 16;
    day             = ((uint16_t) u32tmp) / 2141;

    /*
     * Recall that January 01 is the 306-th day of the year in the
     * computational (not Gregorian) calendar.
     */
    is_Jan_or_Feb   = day_of_year >= 306;

    /* Converts to the Gregorian calendar. */
    year            = year + is_Jan_or_Feb;
    month           = is_Jan_or_Feb ? month - 12 : month;
    day             = day + 1;

    day_of_year     = is_Jan_or_Feb ?
                      day_of_year - 306 :
                      day_of_year + 31 + 28 + is_leap_year;

    tm->tm_year     = (int) (year - 1900);
    tm->tm_mon      = (int) month;
    tm->tm_mday     = (int) day;
    tm->tm_yday     = (int) day_of_year + 1;

    tm->tm_hour     = secs / 3600;
    secs            -= tm->tm_hour * 3600;
    tm->tm_min      = secs / 60;
    tm->tm_sec      = secs - tm->tm_min * 60;
    tm->tm_isdst    = 0;
}
EXPORT_SYMBOL(rtc_time_to_tm);

/**
 * rtc_timespec_to_tm - converts timespec to rtc_time
 * @time: number of nanosecond since 01-01-1970 00:00:00
 * @tm: pointer to the struct rtc_time
 */
void rtc_timespec_to_tm(struct timespec *ts, struct rtc_time *tm)
{
    rtc_time_to_tm(ts->tv_sec, tm);
    tm->tm_nsec = ts->tv_nsec;
}
EXPORT_SYMBOL(rtc_timespec_to_tm);

/**
 * rtc_ktime_to_tm - converts ktime_t to rtc_time
 * @time: number of nanosecond since 01-01-1970 00:00:00
 * @tm: pointer to the struct rtc_time
 */
void rtc_ktime_to_tm(ktime_t ktime, struct rtc_time *tm)
{
	struct timespec ts;

	ts = ktime_to_timespec(ktime);
    rtc_timespec_to_tm(&ts, tm);
}
EXPORT_SYMBOL(rtc_ktime_to_tm);

/**
 * rtc_tm_to_time - converts rtc_time to time_t
 * @tm: pointer to the struct rtc_time
 */
time_t rtc_tm_to_time(struct rtc_time *tm)
{
    return mktime(
        ((unsigned int)tm->tm_year + 1900),
        tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec
    );
}
EXPORT_SYMBOL(rtc_tm_to_time);

/**
 * rtc_tm_to_timespec - converts rtc_time to timespec
 * @tm: pointer to the struct rtc_time
 */
struct timespec rtc_tm_to_timespec(struct rtc_time *tm)
{
    struct timespec ts;

    ts.tv_sec = rtc_tm_to_time(tm);
    ts.tv_nsec = tm->tm_nsec;

    return ts;
}
EXPORT_SYMBOL(rtc_tm_to_timespec);

/**
 * rtc_tm_to_ktime - converts rtc_time to ktime_t
 * @tm: pointer to the struct rtc_time
 */
ktime_t rtc_tm_to_ktime(struct rtc_time *tm)
{
    struct timespec ts;
    ktime_t ktime;

    ts = rtc_tm_to_timespec(tm);
    ktime = timespec_to_ktime(&ts);

    return ktime;
}
EXPORT_SYMBOL(rtc_tm_to_ktime);
