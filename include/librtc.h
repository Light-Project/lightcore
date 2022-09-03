/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIBRTC_H_
#define _LIBRTC_H_

#include <ktime.h>
#include <lightcore/rtc.h>

extern const char *rtc_month_name[12];
extern const char *rtc_wday_name[7];

extern bool rtc_leap_year(unsigned int year);
extern int rtc_month_days(unsigned int year, unsigned int month);
extern int rtc_year_days(unsigned int year, unsigned int month, unsigned int day);
extern void rtc_time_to_tm(time_t time, struct rtc_time *tm);
extern void rtc_timespec_to_tm(struct timespec *ts, struct rtc_time *tm);
extern void rtc_ktime_to_tm(ktime_t ktime, struct rtc_time *tm);
extern time_t rtc_tm_to_time(struct rtc_time *tm);
extern struct timespec rtc_tm_to_timespec(struct rtc_time *tm);
extern ktime_t rtc_tm_to_ktime(struct rtc_time *tm);

#endif  /* _LIBRTC_H_ */
