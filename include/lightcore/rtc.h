#ifndef _LIGHTCORE_RTC_H_
#define _LIGHTCORE_RTC_H_

struct rtc_time {
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
    char enable;
    char pending;  
    struct rtc_time time;    
};

#endif
