/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RTC_MC146818_H_
#define _DRIVER_RTC_MC146818_H_

#include <bits.h>

enum MC146818_Index {
    MC146818_Seconds        = 0x00,
    MC146818_Seconds_Alarm  = 0x01,
    MC146818_Minutes        = 0x02,
    MC146818_Minutes_Alarm  = 0x03,
    MC146818_Hours          = 0x04,
    MC146818_Hours_Alarm    = 0x05,
    MC146818_Day_of_Week    = 0x06,
    MC146818_Day_of_Month   = 0x07,
    MC146818_Month          = 0x08,
    MC146818_Year           = 0x09,
    MC146818_Register_A     = 0x0a,
    MC146818_Register_B     = 0x0b,
    MC146818_Register_C     = 0x0c,
    MC146818_Register_D     = 0x0d,
};

/****************************************************************************************/
/*      Mnemonic                    value       meaning/usage                           */

#define MC146818_Register_A_UIP     BIT(7)  /* Update in progress */
#define MC146818_Register_A_DV2     BIT(6)  /*  */
#define MC146818_Register_A_DV1     BIT(5)  /*  */
#define MC146818_Register_A_DV0     BIT(4)  /*  */
#define MC146818_Register_A_RS3     BIT(3)  /*  */
#define MC146818_Register_A_RS2     BIT(2)  /*  */
#define MC146818_Register_A_RS1     BIT(1)  /*  */
#define MC146818_Register_A_RS0     BIT(0)  /*  */

#define MC146818_Register_B_SET     BIT(7)  /* disable updates for clock setting */
#define MC146818_Register_B_PIE     BIT(6)  /* periodic interrupt enable */
#define MC146818_Register_B_AIE     BIT(5)  /* alarm interrupt enable */
#define MC146818_Register_B_UIE     BIT(4)  /* update-finished interrupt enable */
#define MC146818_Register_B_SQWE    BIT(3)  /* enable square-wave output */
#define MC146818_Register_B_DM      BIT(2)  /* all time/date values are BCD if clear */
#define MC146818_Register_B_24H     BIT(1)  /* 24 hour mode - else hours bit 7 means pm */
#define MC146818_Register_B_DST     BIT(0)  /* auto switch DST - works f. USA only */

#define MC146818_Register_C_IRQF    BIT(7)
#define MC146818_Register_C_PF      BIT(6)
#define MC146818_Register_C_AF      BIT(5)
#define MC146818_Register_C_UF      BIT(4)

#define MC146818_Register_D_VRT     BIT(7)

#endif /* _DRIVER_RTC_MC146818_H_ */
