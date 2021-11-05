/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PLATFORM_GX6605S_H_
#define _DRIVER_PLATFORM_GX6605S_H_

#include <bits.h>

enum gx6605s_gctl_registers {
    GX6605S_MPEG_CLD_RST1_DAT       = 0x00,
    GX6605S_MPEG_CLD_RST1_SET       = 0x04,
    GX6605S_MPEG_CLD_RST1_CLR       = 0x08,

    GX6605S_MPEG_HOT_RST1_DAT       = 0x0c,
    GX6605S_MPEG_HOT_RST1_SET       = 0x10,
    GX6605S_MPEG_HOT_RST1_CLR       = 0x14,

    GX6605S_MPEG_CLK_INHIBIT_DAT    = 0x18,
    GX6605S_MPEG_CLK_INHIBIT_SET    = 0x1c,
    GX6605S_MPEG_CLK_INHIBIT_CLR    = 0x20,

    GX6605S_DTO1_CONFIG	            = 0x28,
    GX6605S_DTO2_CONFIG	            = 0x2C,
    GX6605S_DTO3_CONFIG	            = 0x30,
    GX6605S_DTO4_CONFIG	            = 0x34,
    GX6605S_DTO5_CONFIG	            = 0x38,
    GX6605S_DTO6_CONFIG	            = 0x3C,
    GX6605S_DTO7_CONFIG	            = 0x40,
    GX6605S_DTO8_CONFIG	            = 0x44,
    GX6605S_DTO9_CONFIG	            = 0x48,
    GX6605S_DTO10_CONFIG            = 0x4C,
    GX6605S_DTO11_CONFIG            = 0x50,
    GX6605S_DTO12_CONFIG            = 0x54,
    GX6605S_DTO13_CONFIG            = 0x58,
    GX6605S_DTO14_CONFIG            = 0x5C,
    GX6605S_DTO15_CONFIG            = 0x60,
    GX6605S_DTO16_CONFIG            = 0x64,

    GX6605S_MPEG_CLK_INHIBIT2_NORM  = 0x68,
    GX6605S_MPEG_CLK_INHIBIT2_1SET  = 0x6c,
    GX6605S_MPEG_CLK_INHIBIT2_1CLR  = 0x70,

    GX6605S_PLL_CPU_BASE            = 0x0c0,
    GX6605S_PLL_DTO_BASE            = 0x0c8,
    GX6605S_PLL_DVB_BASE            = 0x0d0,
    GX6605S_PLL_DDR_BASE            = 0x0e0,

    GX6605S_USB_CONFIG              = 0x100,
    GX6605S_USB1_CONFIG             = 0x104,
    GX6605S_USB2_CONFIG             = 0x108,
    GX6605S_USB3_CONFIG             = 0x10c,

    GX6605S_DVB_CONFIG              = 0x110,
    GX6605S_EPHY_CONFIG             = 0x114,

    GX6605S_DRAM_CTRL0              = 0x120,
    GX6605S_DRAM_CTRL1              = 0x124,
    GX6605S_DRAM_CTRL2              = 0x128,

    GX6605S_PINMUX_PORTA            = 0x13C,
    GX6605S_PINMUX_PORTB            = 0x140,
    GX6605S_PINMUX_PORTC            = 0x144,
    GX6605S_PINMUX_PORTD            = 0x148,

    GX6605S_MMU_CONGIG              = 0x164,

    GX6605S_SOURCE_SEL1             = 0x170,
    GX6605S_SOURCE_SEL2             = 0x174,
    GX6605S_CLOCK_DIV_CONFIG2       = 0x178,
    GX6605S_CLOCK_DIV_CONFIG3       = 0x17C,

    GX6605S_AUDIO_CODEC_DATA        = 0x1A0,
    GX6605S_AUDIO_CODEC_CONTROL     = 0x1A4,

    GX6605S_MPEG_CLD_RST2_DAT       = 0x1c0,
    GX6605S_MPEG_CLD_RST2_SET       = 0x1c4,
    GX6605S_MPEG_CLD_RST2_CLR       = 0x1c8,

    GX6605S_MPEG_HOT_RST2_DAT       = 0x1d0,
    GX6605S_MPEG_HOT_RST2_SET       = 0x1d4,
    GX6605S_MPEG_HOT_RST2_CLR       = 0x1d8,

    GX6605S_HDMI_REG_DATA           = 0x1E0,
    GX6605S_HDMI_REG_CONTROL        = 0x1E4,
    GX6605S_HDMI_KEY                = 0x1E8,
    GX6605S_DAC                     = 0x1F0,
    GX6605S_ADC                     = 0x200,

    GX6605S_STATE_ADC               = 0x204,

    GX6605S_DENALI_CONFIG0          = 0x580,
    GX6605S_CPU_32BIT_ANLIGN        = 0x600,
};

enum gx6605s_rcc_registers {
    GX6605S_RCC_CTRL_REG            = 0x00,
    GX6605S_RCC_STA_REG             = 0x04,
    GX6605S_RCC_INTERVAL            = 0x08,
    GX6605S_RCC_START_ADDR0         = 0x10,
    GX6605S_RCC_SIZE0               = 0x14,
    GX6605S_RCC_START_ADDR1         = 0x18,
    GX6605S_RCC_SIZE1               = 0x1c,
    GX6605S_RCC_START_ADDR2         = 0x20,
    GX6605S_RCC_SIZE2               = 0x24,
    GX6605S_RCC_START_ADDR3         = 0x28,
    GX6605S_RCC_SIZE3               = 0x2c,
    GX6605S_HASH_START_ADDR         = 0x40,
    GX6605S_HASH_SIZE               = 0x44,
    GX6605S_RCC_HASH_A              = 0x50,
    GX6605S_RCC_HASH_B              = 0x54,
    GX6605S_RCC_HASH_C              = 0x58,
    GX6605S_RCC_HASH_D              = 0x5c,
    GX6605S_RCC_HASH_E              = 0x60,
    GX6605S_RCC_HASH_F              = 0x64,
    GX6605S_RCC_HASH_G              = 0x68,
    GX6605S_RCC_HASH_H              = 0x6c,
};

#define GX6605S_DRAM_CTRL0_DM_OCD       BIT_RANGE(26, 24)
#define GX6605S_DRAM_CTRL0_DQ_PD        BIT_RANGE(23, 20)
#define GX6605S_DRAM_CTRL0_DQ_PU        BIT_RANGE(19, 16)
#define GX6605S_DRAM_CTRL0_FEED_OCD2    BIT_RANGE(5, 3)
#define GX6605S_DRAM_CTRL0_FEED_OCD1    BIT_RANGE(5, 3)
#define GX6605S_DRAM_CTRL0_FEED_ODT     BIT_RANGE(2, 0)

#define GX6605S_DRAM_CTRL1_CLK_PD       BIT_RANGE(21, 18)
#define GX6605S_DRAM_CTRL1_CLK_PU       BIT_RANGE(17, 14)
#define GX6605S_DRAM_CTRL1_
#define GX6605S_DRAM_CTRL1_DQS_PD
#define GX6605S_DRAM_CTRL1_DQS_PU
#define GX6605S_DRAM_CTRL1_DS_OCD

#define GX6605S_DRAM_CTRL2_VREF         BIT_RANGE(19, 16)
#define GX6605S_DRAM_CTRL2_ADDR_PD      BIT_RANGE(10, 7)
#define GX6605S_DRAM_CTRL2_ADDR_PU      BIT_RANGE(6, 3)

#endif  /* _DRIVER_PLATFORM_GX6605S_H_ */
