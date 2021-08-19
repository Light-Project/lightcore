/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CHIP_GPIO_SUNXI_H_
#define _CHIP_GPIO_SUNXI_H_

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
};

#endif  /* _CHIP_GPIO_SUNXI_H_ */
