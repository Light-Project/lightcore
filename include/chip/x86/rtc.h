/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/rtc.h
 *  x86 rtc register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */

#ifndef _CHIP_X86_RTC_H_ 
#define _CHIP_X86_RTC_H_

#include <types.h>

enum{
    Rtc_Seconds         = 0x00,
    Rtc_Seconds_Alarm   = 0x01,
    Rtc_Minutes         = 0x02,
    Rtc_Minutes_Alarm   = 0x03,
    Rtc_Hours           = 0x04,
    Rtc_Hours_Alarm     = 0x05,
    Rtc_Day_of_Week     = 0x06,
    Rtc_Day_of_Month    = 0x07,
    Rtc_Month           = 0x08,
    Rtc_Year            = 0x09,
    Rtc_RTC_REGA        = 0x0a,
    Rtc_Register_B      = 0x0b,
    Rtc_Register_C      = 0x0c, 
    Rtc_Register_D      = 0x0d,
};

typedef struct chip_rtc{
    uint8_t Seconds;
    uint8_t Seconds_Alarm;
    uint8_t Minutes;
    uint8_t Minutes_Alarm;
    uint8_t Hours;
    uint8_t Hours_Alarm;
    uint8_t Day_of_Week;
    uint8_t Day_of_Month;
    uint8_t Month;
    uint8_t Year;
    uint8_t RTC_REGA;
    uint8_t Register_B;
    uint8_t Register_C;
    uint8_t Register_D;
} chip_rtc_t;

#endif /* _CHIP_X86_RTC_H_ */
