/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CPUFREQ_CK505_H_
#define _DRIVER_CPUFREQ_CK505_H_

#include <bits.h>

enum ck404_registers {
    CK505_FS_PLL        = 0x00,     /* RW: FS Readback and PLL Selection Register */
    CK505_DOT96_PLL3    = 0x01,     /* RW: DOT96 Select and PLL3 Quick Config Register */
    CK505_OUTEN0        = 0x02,     /* RW: Output Enable Register */
    CK505_OUTEN1        = 0x03,     /* RW: Output Enable Register */
    CK505_OUTEN2_SSD    = 0x04,     /* RW: Output Enable and Spread Spectrum Disable Register */
    CK505_CLKREQ_CONF   = 0x05,     /* RW: Clock Request Enable/Configuration Register */
    CK505_CLKREQ_STOP   = 0x06,     /* RW: Clock Request Enable/Configuration and Stop Control Register */
    CK505_VENDOR        = 0x07,     /* RO: Vendor ID/ Revision ID */
    CK505_DEVID_OUTEN3  = 0x08,     /* RW: Device ID and Output Enable Register */
    CK505_OUTCTL        = 0x09,     /* RW: Output Control Register */
    CK505_FREE_RUNNING  = 0x0a,     /* RW: Free-Running Control Register */
    CK505_STRENGTH      = 0x0b,     /* RW: Strength Control Register */
    CK505_COUNTER       = 0x0c,     /* RW: Byte Count Register */
    CK505_PLL1_VCO0     = 0x0d,     /* RW: VCO Frequency Control Register PLL1 */
    CK505_PLL1_VCO1     = 0x0e,     /* RW: VCO Frequency Control Register PLL1 */
    CK505_PLL1_SS0      = 0x0f,     /* RW: Spread Spectrum Control Register PLL1 */
    CK505_PLL1_SS1      = 0x10,     /* RW: Spread Spectrum Control Register PLL1 */
    CK505_PLL3_VCO0     = 0x11,     /* RW: VCO Frequency Control Register PLL3 */
    CK505_PLL3_VCO1     = 0x12,     /* RW: VCO Frequency Control Register PLL3 */
    CK505_PLL3_SS0      = 0x13,     /* RW: Spread Spectrum Control Register PLL3 */
    CK505_PLL3_SS1      = 0x14,     /* RW: Spread Spectrum Control Register PLL3 */
    CK505_MN_EN         = 0x15,     /* RW: M/N Enables */
};

#endif  /* _DRIVER_CPUFREQ_CK505_H_ */
