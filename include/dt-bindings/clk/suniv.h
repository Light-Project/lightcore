/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DT_BINDINGS_CLK_SUNIV_H_
#define _DT_BINDINGS_CLK_SUNIV_H_

#define SUNIV_PLL_CPU       0
#define SUNIV_PLL_AUDIO     1
#define SUNIV_PLL_VIDEO     2
#define SUNIV_PLL_VE        3
#define SUNIV_PLL_DDR       4
#define SUNIV_PLL_PERIPH    5

/* AHB-Bus */
#define SUNIV_CLK_DMA       6
#define SUNIV_CLK_MMC0      8
#define SUNIV_CLK_MMC1      9
#define SUNIV_CLK_SDRAM     14
#define SUNIV_CLK_SPI0      20
#define SUNIV_CLK_SPI1      21
#define SUNIV_CLK_OTG       24

#define SUNIV_CLK_VE        32
#define SUNIV_CLK_LCD       36
#define SUNIV_CLK_DE        37
#define SUNIV_CLK_CSI       40
#define SUNIV_CLK_TVD       41
#define SUNIV_CLK_TVE       42
#define SUNIV_CLK_DEBE      44
#define SUNIV_CLK_DEFE      46

/* APB-Bus */
#define SUNIV_CLK_CODEC     64
#define SUNIV_CLK_OWA       65
#define SUNIV_CLK_CIR       66
#define SUNIV_CLK_RSB       67
#define SUNIV_CLK_DAUDIO    76
#define SUNIV_CLK_TWI0      80
#define SUNIV_CLK_TWI1      81
#define SUNIV_CLK_TWI2      82
#define SUNIV_CLK_UART0     84
#define SUNIV_CLK_UART1     85
#define SUNIV_CLK_UART2     86

#endif  /* _DT_BINDINGS_CLK_SUNIV_H_ */
