/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPIO_SUNXI_H_
#define _DRIVER_GPIO_SUNXI_H_

enum sunxi_gpio_registers {
    SUNXI_GPIO_CFG0         = 0x00,     /* n * 0x24 + (0x00) */
    SUNXI_GPIO_CFG1         = 0x04,     /* n * 0x24 + (0x04) */
    SUNXI_GPIO_CFG2         = 0x08,     /* n * 0x24 + (0x08) */
    SUNXI_GPIO_CFG3         = 0x0c,     /* n * 0x24 + (0x0c) */
    SUNXI_GPIO_DATA         = 0x10,     /* n * 0x24 + (0x10) */
    SUNXI_GPIO_DRV0         = 0x14,     /* n * 0x24 + (0x14) */
    SUNXI_GPIO_DRV1         = 0x18,     /* n * 0x24 + (0x18) */
    SUNXI_GPIO_PUL0         = 0x1c,     /* n * 0x24 + (0x1c) */
    SUNXI_GPIO_PUL1         = 0x20,     /* n * 0x24 + (0x20) */

    SUNXI_GPIO_INT_CFG0     = 0x200,    /* n * 0x20 + (0x200 + 0x00) */
    SUNXI_GPIO_INT_CFG1     = 0x204,    /* n * 0x20 + (0x200 + 0x04) */
    SUNXI_GPIO_INT_CFG2     = 0x208,    /* n * 0x20 + (0x200 + 0x08) */
    SUNXI_GPIO_INT_CFG3     = 0x20c,    /* n * 0x20 + (0x200 + 0x0c) */
    SUNXI_GPIO_INT_CTRL     = 0x210,    /* n * 0x20 + (0x200 + 0x10) */
    SUNXI_GPIO_INT_STA      = 0x214,    /* n * 0x20 + (0x200 + 0x14) */
    SUNXI_GPIO_INT_DEB      = 0x218,    /* n * 0x20 + (0x200 + 0x18) */

    SUNIV_GPIO_SDR_DRV      = 0x2c0,
    SUNIV_GPIO_SDR_PUL      = 0x2c4,
};

enum sunxi_iobase {
    SUNXI_PA_BASE   = 0,
    SUNXI_PB_BASE   = 32,
    SUNXI_PC_BASE   = 64,
    SUNXI_PD_BASE   = 96,
    SUNXI_PE_BASE   = 128,
    SUNXI_PF_BASE   = 160,
    SUNXI_PG_BASE   = 192,
    SUNXI_PH_BASE   = 224,
    SUNXI_PI_BASE   = 256,
    SUNXI_PL_BASE   = 352,
    SUNXI_PM_BASE   = 384,
    SUNXI_PN_BASE   = 416,
};

#define SUNXI_GPIO_CFG_INPUT    0x00
#define SUNXI_GPIO_CFG_OUTPUT   0x01
#define SUNXI_GPIO_CFG_FUN2     0x02
#define SUNXI_GPIO_CFG_FUN3     0x03
#define SUNXI_GPIO_CFG_FUN4     0x04
#define SUNXI_GPIO_CFG_FUN5     0x05
#define SUNXI_GPIO_CFG_FUN6     0x06
#define SUNXI_GPIO_CFG_DISABLE  0x07

#define SUNXI_GPIO_DRV_LEVEL0   0x00
#define SUNXI_GPIO_DRV_LEVEL1   0x01
#define SUNXI_GPIO_DRV_LEVEL2   0x02
#define SUNXI_GPIO_DRV_LEVEL3   0x03

#endif /* _CHIP_GPIO_SUNXI_H_ */
