/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_PL11X_H_
#define _DRIVER_VIDEO_PL11X_H_

#include <bits.h>

enum pl11x_registers {
    PL11X_TIM0      = 0x00,
    PL11X_TIM1      = 0x04,
    PL11X_TIM2      = 0x08,
    PL11X_TIM3      = 0x0c,
    PL11X_UBAS      = 0x10,
    PL11X_LBAS      = 0x14,

    PL110_IENB      = 0x18,
    PL110_CNTL      = 0x1c,
    PL110_STAT      = 0x20,
    PL110_INTR      = 0x24,
    PL110_UCUR      = 0x28,
    PL110_LCUR      = 0x2C,

    PL111_CNTL      = 0x18,
    PL111_IENB      = 0x1c,
    PL111_RIS       = 0x20,
    PL111_MIS       = 0x24,
    PL111_ICR       = 0x28,
    PL111_UCUR      = 0x2c,
    PL111_LCUR      = 0x30,
};

#define PL11X_TIM2_PCD_HI       BIT_RANGE(31, 27)
#define PL11X_TIM2_BCD          BIT(26)
#define PL11X_TIM2_IOE          BIT(14)
#define PL11X_TIM2_IPC          BIT(13)
#define PL11X_TIM2_IHS          BIT(12)
#define PL11X_TIM2_IVS          BIT(11)
#define PL11X_TIM2_ACB          BIT_RANGE(10, 6)
#define PL11X_TIM2_CLKSEL       BIT(5)
#define PL11X_TIM2_PCD_LO       BIT_RANGE(4, 0)

#define PL11X_CNTL_WATERMARK    BIT(16)
#define PL11X_CNTL_LDMAFIFOTIME BIT(15)
#define PL11X_CNTL_LCDVCOMP(x)  ((x) << 12)
#define PL11X_CNTL_LCDPWR       BIT(11)
#define PL11X_CNTL_BEPO         BIT(10)
#define PL11X_CNTL_BEBO         BIT(9)
#define PL11X_CNTL_BGR          BIT(8)
#define PL11X_CNTL_LCDDUAL      BIT(7)
#define PL11X_CNTL_LCDMONO8     BIT(6)
#define PL11X_CNTL_LCDTFT       BIT(5)
#define PL11X_CNTL_LCDBW        BIT(4)
#define PL11X_CNTL_LCDBPP24     (5 << 1)
#define PL11X_CNTL_LCDBPP16_565 (6 << 1)
#define PL11X_CNTL_LCDBPP16_444 (7 << 1)
#define PL11X_CNTL_LCDBPP16     (4 << 1)
#define PL11X_CNTL_LCDBPP8      (3 << 1)
#define PL11X_CNTL_LCDBPP4      (2 << 1)
#define PL11X_CNTL_LCDBPP2      (1 << 1)
#define PL11X_CNTL_LCDBPP1      (0 << 1)
#define PL11X_CNTL_LCDEN        BIT(0)

/* ST Microelectronics variant bits */
#define PL11X_CNTL_LCDBPP24_PACKED  (6 << 1)
#define PL11X_CNTL_CEAEN        BIT(21)
#define PL11X_CNTL_CDWID_24     BIT_RANGE(20, 19)
#define PL11X_CNTL_CDWID_18     BIT(20)
#define PL11X_CNTL_CDWID_16     BIT(19)
#define PL11X_CNTL_1XBPP_565    BIT(18)
#define PL11X_CNTL_1XBPP_5551   BIT(17)

#endif /* _DRIVER_VIDEO_PL11X_H_ */
