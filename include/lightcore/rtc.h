/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_RTC_H_
#define _LIGHTCORE_RTC_H_

#include <lightcore/types.h>

struct rtc_time {
    int tm_nsec;
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

struct rtc_alarm {
    struct rtc_time time;
    unsigned char enable:1;
    unsigned char pending:1;
};

#endif  /* _LIGHTCORE_RTC_H_ */
