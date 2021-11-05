/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_RTC_H_
#define _LIGHTCORE_RTC_H_

#include <lightcore/types.h>

struct rtc_time {
    unsigned int tm_us;
    unsigned int tm_ms;
    unsigned int tm_sec;
    unsigned int tm_min;
    unsigned int tm_hour;
    unsigned int tm_mday;
    unsigned int tm_mon;
    unsigned int tm_year;
    unsigned int tm_wday;
    unsigned int tm_yday;
    unsigned int tm_isdst;
};

struct rtc_alarm {
    struct rtc_time time;
    uint8_t enable:1;
    uint8_t pending:1;
};

#endif  /* _LIGHTCORE_RTC_H_ */
