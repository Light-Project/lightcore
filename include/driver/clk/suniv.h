/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLK_SUNIV_H_
#define _DRIVER_CLK_SUNIV_H_

#include <bits.h>

/*
 * The CCU provides the registers to program the PLLs and the controls most of
 * the clock generation, division, distribution, synchronization and gating.
 * CCU input signal is the external clock for the reference frequency (24MHz).
 * The outputs from CCU are mostly clocks to the other blocks in the system.
 */

enum suniv_ccu_registers {
    SUNIV_PLL_CPU_CTRL           = 0x00,
    SUNIV_PLL_AUDIO_CTRL         = 0x08,
    SUNIV_PLL_VIDEO_CTRL         = 0x10,
    SUNIV_PLL_VE_CTRL            = 0x18,
    SUNIV_PLL_DDR_CTRL           = 0x20,
    SUNIV_PLL_PERIPH_CTRL        = 0x28,
    SUNIV_CPU_CLK_SRC            = 0x50,
    SUNIV_AHB_APB_HCLKC_CFG      = 0x54,
    SUNIV_BUS_CLK_GATING_REG0    = 0x60,
    SUNIV_BUS_CLK_GATING_REG1    = 0x64,
    SUNIV_BUS_CLK_GATING_REG2    = 0x68,
    SUNIV_SDMMC0_CLK             = 0x88,
    SUNIV_SDMMC1_CLK             = 0x8c,
    SUNIV_DAUDIO_CLK             = 0xb0,
    SUNIV_OWA_CLK                = 0xb4,
    SUNIV_CIR_CLK                = 0xb8,
    SUNIV_USBPHY_CLK             = 0xcc,
    SUNIV_DRAM_GATING            = 0x100,
    SUNIV_BE_CLK                 = 0x104,
    SUNIV_FE_CLK                 = 0x10c,
    SUNIV_TCON_CLK               = 0x118,
    SUNIV_DI_CLK                 = 0x11c,
    SUNIV_TVE_CLK                = 0x120,
    SUNIV_TVD_CLK                = 0x124,
    SUNIV_CSI_CLK                = 0x134,
    SUNIV_VE_CLK                 = 0x13c,
    SUNIV_AUDIO_CODEC_CLK        = 0x140,
    SUNIV_AVS_CLK                = 0x144,
    SUNIV_PLL_STABLE_TIME_REG0   = 0x200,
    SUNIV_PLL_STABLE_TIME_REG1   = 0x204,
    SUNIV_PLL_CPU_BIAS           = 0x220,
    SUNIV_PLL_AUDIO_BIAS         = 0x224,
    SUNIV_PLL_VIDEO_BIAS         = 0x228,
    SUNIV_PLL_VE_BIAS            = 0x22c,
    SUNIV_PLL_DDR_BIAS           = 0x230,
    SUNIV_PLL_PERIPH_BIAS        = 0x234,
    SUNIV_PLL_CPU_TUN            = 0x250,
    SUNIV_PLL_DDR_TUN            = 0x260,
    SUNIV_PLL_AUDIO_PAT_CTRL     = 0x284,
    SUNIV_PLL_VIDEO_PAT_CTRL     = 0x288,
    SUNIV_PLL_DDR_PAT_CTRL       = 0x290,
    SUNIV_BUS_SOFT_RST_REG0      = 0x2c0,
    SUNIV_BUS_SOFT_RST_REG1      = 0x2c4,
    SUNIV_BUS_SOFT_RST_REG2      = 0x2d0,
};

#endif  /* _DRIVER_CLK_SUNIV_H_ */
