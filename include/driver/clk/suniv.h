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
    SUNIV_PLL_CPU_CTRL          = 0x0,
    SUNIV_PLL_AUDIO_CTRL        = 0x8,
    SUNIV_PLL_VIDEO_CTRL        = 0x10,
    SUNIV_PLL_VE_CTRL           = 0x18,
    SUNIV_PLL_DDR_CTRL          = 0x20,
    SUNIV_PLL_PERIPH_CTRL       = 0x28,
    SUNIV_CPU_CLK_SRC           = 0x50,
    SUNIV_AHB_APB_HCLKC_CFG     = 0x54,
    SUNIV_BUS_CLK_GATING_REG0   = 0x60,
    SUNIV_BUS_CLK_GATING_REG1   = 0x64,
    SUNIV_BUS_CLK_GATING_REG2   = 0x68,
    SUNIV_SDMMC0_CLK            = 0x88,
    SUNIV_SDMMC1_CLK            = 0x8c,
    SUNIV_DAUDIO_CLK            = 0xb0,
    SUNIV_OWA_CLK               = 0xb4,
    SUNIV_CIR_CLK               = 0xb8,
    SUNIV_USBPHY_CLK            = 0xcc,
    SUNIV_DRAM_GATING           = 0x100,
    SUNIV_BE_CLK                = 0x104,
    SUNIV_FE_CLK                = 0x10c,
    SUNIV_TCON_CLK              = 0x118,
    SUNIV_DI_CLK                = 0x11c,
    SUNIV_TVE_CLK               = 0x120,
    SUNIV_TVD_CLK               = 0x124,
    SUNIV_CSI_CLK               = 0x134,
    SUNIV_VE_CLK                = 0x13c,
    SUNIV_AUDIO_CODEC_CLK       = 0x140,
    SUNIV_AVS_CLK               = 0x144,
    SUNIV_PLL_STABLE_TIME_REG0  = 0x200,
    SUNIV_PLL_STABLE_TIME_REG1  = 0x204,
    SUNIV_PLL_CPU_BIAS          = 0x220,
    SUNIV_PLL_AUDIO_BIAS        = 0x224,
    SUNIV_PLL_VIDEO_BIAS        = 0x228,
    SUNIV_PLL_VE_BIAS           = 0x22c,
    SUNIV_PLL_DDR_BIAS          = 0x230,
    SUNIV_PLL_PERIPH_BIAS       = 0x234,
    SUNIV_PLL_CPU_TUN           = 0x250,
    SUNIV_PLL_DDR_TUN           = 0x260,
    SUNIV_PLL_AUDIO_PAT_CTRL    = 0x284,
    SUNIV_PLL_VIDEO_PAT_CTRL    = 0x288,
    SUNIV_PLL_DDR_PAT_CTRL      = 0x290,
    SUNIV_BUS_SOFT_RST_REG0     = 0x2c0,
    SUNIV_BUS_SOFT_RST_REG1     = 0x2c4,
    SUNIV_BUS_SOFT_RST_REG2     = 0x2d0,
};

#define SUNIV_BUS0_DMA          BIT(6)
#define SUNIV_BUS0_MMC0         BIT(8)
#define SUNIV_BUS0_MMC1         BIT(9)
#define SUNIV_BUS0_SDRAM        BIT(14)
#define SUNIV_BUS0_SPI0         BIT(20)
#define SUNIV_BUS0_SPI1         BIT(21)
#define SUNIV_BUS0_OTG          BIT(24)

#define SUNIV_BUS1_VE           BIT(0)
#define SUNIV_BUS1_LCD          BIT(4)
#define SUNIV_BUS1_DE           BIT(5)
#define SUNIV_BUS1_CSI          BIT(8)
#define SUNIV_BUS1_TVD          BIT(9)
#define SUNIV_BUS1_TVE          BIT(10)
#define SUNIV_BUS1_DEBE         BIT(12)
#define SUNIV_BUS1_DEFE         BIT(14)

#define SUNIV_BUS2_CODEC        BIT(0)
#define SUNIV_BUS2_OWA          BIT(1)
#define SUNIV_BUS2_CIR          BIT(2)
#define SUNIV_BUS2_RSB          BIT(3)
#define SUNIV_BUS2_DAUDIO       BIT(12)
#define SUNIV_BUS2_TWI0         BIT(16)
#define SUNIV_BUS2_TWI1         BIT(17)
#define SUNIV_BUS2_TWI2         BIT(18)
#define SUNIV_BUS2_UART0        BIT(20)
#define SUNIV_BUS2_UART1        BIT(21)
#define SUNIV_BUS2_UART2        BIT(22)

enum suniv_dramc {
    SUNIV_DRAMC_SCON            = 0x0,
    SUNIV_DRAMC_STMG0           = 0x4,
    SUNIV_DRAMC_STMG1           = 0x8,
    SUNIV_DRAMC_SCTL            = 0xc,
    SUNIV_DRAMC_SREF            = 0x10,
    SUNIV_DRAMC_SEXTM           = 0x14,
    SUNIV_DRAMC_DDLY            = 0x24,
    SUNIV_DRAMC_DADR            = 0x28,
    SUNIV_DRAMC_DVAL            = 0x2c,
    SUNIV_DRAMC_DRPTR0          = 0x30,
    SUNIV_DRAMC_DRPTR1          = 0x34,
    SUNIV_DRAMC_DRPTR2          = 0x38,
    SUNIV_DRAMC_DRPTR3          = 0x3c,
    SUNIV_DRAMC_SEFR            = 0x40,
    SUNIV_DRAMC_MAE             = 0x44,
    SUNIV_DRAMC_ASPR            = 0x48,
    SUNIV_DRAMC_SDLY0           = 0x4C,
    SUNIV_DRAMC_SDLY1           = 0x50,
    SUNIV_DRAMC_SDLY2           = 0x54,
    SUNIV_DRAMC_MCR0            = 0x100,
    SUNIV_DRAMC_MCR1            = 0x104,
    SUNIV_DRAMC_MCR2            = 0x108,
    SUNIV_DRAMC_MCR3            = 0x10c,
    SUNIV_DRAMC_MCR4            = 0x110,
    SUNIV_DRAMC_MCR5            = 0x114,
    SUNIV_DRAMC_MCR6            = 0x118,
    SUNIV_DRAMC_MCR7            = 0x11c,
    SUNIV_DRAMC_MCR8            = 0x120,
    SUNIV_DRAMC_MCR9            = 0x124,
    SUNIV_DRAMC_MCR10           = 0x128,
    SUNIV_DRAMC_MCR11           = 0x12c,
    SUNIV_DRAMC_BWCR            = 0x140,
};

#define SUNIV_DRAMC_SCON_ACCELL     BIT(15)     // Dram Accell Mode (0: interleave mode 1: sequence mode)
#define SUNIV_DRAMC_SCON_CS_DOUBLE  BIT(4)      //
#define SUNIV_DRAMC_SCON_BANK_SIZE  BIT(3)      //
#define SUNIV_DRAMC_SCON_DDR_REMAP  BIT(0)      // DDR Data Width (0: normal  1: 8bits)

#define SUNIV_DRAMC_STMG0_RC        (0x0b<<28)
#define SUNIV_DRAMC_STMG0_XSR       (0xf9<<19)
#define SUNIV_DRAMC_STMG0_RFC       (0x0d<<15)
#define SUNIV_DRAMC_STMG0_WR        (0x03<<13)
#define SUNIV_DRAMC_STMG0_RP        (0x03<<10)
#define SUNIV_DRAMC_STMG0_RCD       (0x03<<7)
#define SUNIV_DRAMC_STMG0_RAS       (0x08<<3)
#define SUNIV_DRAMC_STMG0_CAS       (0x02<<0)

#define SUNIV_DRAMC_STMG1_RRD       (0x02<<25)
#define SUNIV_DRAMC_STMG1_WTR       (0x02<<20)
#define SUNIV_DRAMC_STMG1_REF       (0x07<<16)
#define SUNIV_DRAMC_STMG1_INIT      (0x08<<0)

#endif  /* _DRIVER_CLK_SUNIV_H_ */
