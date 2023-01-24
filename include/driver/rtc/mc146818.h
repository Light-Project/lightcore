/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RTC_MC146818_H_
#define _DRIVER_RTC_MC146818_H_

#include <bits.h>

enum MC146818_Index {
    MC146818_SECONDS        = 0x00,
    MC146818_SECONDS_ALARM  = 0x01,
    MC146818_MINUTES        = 0x02,
    MC146818_MINUTES_ALARM  = 0x03,
    MC146818_HOURS          = 0x04,
    MC146818_HOURS_ALARM    = 0x05,
    MC146818_DAY_OF_WEEK    = 0x06,
    MC146818_DAY_OF_MONTH   = 0x07,
    MC146818_MONTH          = 0x08,
    MC146818_YEAR           = 0x09,
    MC146818_REGISTER_A     = 0x0a,
    MC146818_REGISTER_B     = 0x0b,
    MC146818_REGISTER_C     = 0x0c,
    MC146818_REGISTER_D     = 0x0d,
    MC146818_NVRAM          = 0x0e,
};

#define MC146818_NVRAM(index) (MC146818_NVRAM + (index))
#define MC146818_NVRAM_SIZE (0x100 - MC146818_NVRAM)

/****************************************************************************************/
/*      Mnemonic                    value       meaning/usage                           */

#define MC146818_REGISTER_A_UIP     BIT(7)  /* Update In Progress */
#define MC146818_REGISTER_A_DV2     BIT(6)  /* Divider Selection: Bit 2 */
#define MC146818_REGISTER_A_DV1     BIT(5)  /* Divider Selection: Bit 1 */
#define MC146818_REGISTER_A_DV0     BIT(4)  /* Divider Selection: Bit 0 */
#define MC146818_REGISTER_A_RS3     BIT(3)  /* Rate Selection: Bit 3 */
#define MC146818_REGISTER_A_RS2     BIT(2)  /* Rate Selection: Bit 2 */
#define MC146818_REGISTER_A_RS1     BIT(1)  /* Rate Selection: Bit 1 */
#define MC146818_REGISTER_A_RS0     BIT(0)  /* Rate Selection: Bit 0 */

#define MC146818_REGISTER_B_SET     BIT(7)  /* Disable Updates For Clock Setting */
#define MC146818_REGISTER_B_PIE     BIT(6)  /* Periodic Interrupt Enable */
#define MC146818_REGISTER_B_AIE     BIT(5)  /* Alarm Interrupt Enable */
#define MC146818_REGISTER_B_UIE     BIT(4)  /* Update Finished Interrupt Enable */
#define MC146818_REGISTER_B_SQWE    BIT(3)  /* Enable Square-Wave Output */
#define MC146818_REGISTER_B_DM      BIT(2)  /* All Time/Date Values Are Bcd If Clear */
#define MC146818_REGISTER_B_24H     BIT(1)  /* 24 Hour Mode - Else Hours Bit 7 Means Pm */
#define MC146818_REGISTER_B_DST     BIT(0)  /* Auto Switch Dst - Works F. Usa Only */

#define MC146818_REGISTER_C_IRQF    BIT(7)  /* Interrupt Request Flag */
#define MC146818_REGISTER_C_PF      BIT(6)  /* Periodic Interrupt Flag */
#define MC146818_REGISTER_C_AF      BIT(5)  /* Alarm Interrupt Flag */
#define MC146818_REGISTER_C_UF      BIT(4)  /* Update Ended Interrupt Flag */

#define MC146818_REGISTER_D_VRT     BIT(7)  /* Valid Ram and Time  */

#endif /* _DRIVER_RTC_MC146818_H_ */
