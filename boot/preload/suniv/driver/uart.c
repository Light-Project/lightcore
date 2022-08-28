/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define UART8250_SHIFT 2

#include "boot.h"
#include <driver/clk/suniv.h>
#include <driver/gpio/sunxi.h>
#include <driver/uart/8250.h>

#include <asm/io.h>
#include <asm/proc.h>

void uart_print(const char *str)
{
    while (*str) {
        while (!(readl(SER_BASE + UART8250_LSR) & UART8250_LSR_THRE))
            cpu_relax();
        writel(SER_BASE + UART8250_THR, ch);
    }
}

void uart_sync(void)
{
    while (!(readl(SER_BASE + UART8250_LSR) & UART8250_LSR_TEMT))
        cpu_relax();
}

void uart_init(uint32_t freq)
{
    uint32_t clk = (clock_apb / freq) / 16;
    uint32_t val;

    val = readl(PIO_E + SUNXI_GPIO_CFG0);
    val &= ~(0x0F << 4 | 0x0F << 0);
    val |= (5 << 4)|(5 << 0);
    writel(PIO_E + SUNXI_GPIO_CFG0, val);

    val = readl(CCU_BASE + SUNIV_BUS_CLK_GATING_REG2);
    val |= 1 << 20;
    writel(CCU_BASE + SUNIV_BUS_CLK_GATING_REG2, val);

    val = readl(CCU_BASE + SUNIV_BUS_SOFT_RST_REG2);
    val &= ~(1 << 20);
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG2, val);
    val |= 1 << 20;
    writel(CCU_BASE + SUNIV_BUS_SOFT_RST_REG2, val);

    mdelay(10);

    writel(SER_BASE + UART8250_IER, 0);

    writel(SER_BASE + UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    writel(SER_BASE + UART8250_FCR, UART8250_FCR_FIFO_EN);

    writel(SER_BASE + UART8250_LCR, UART8250_LCR_DLAB);
    writel(SER_BASE + UART8250_DLL, clk);
    writel(SER_BASE + UART8250_DLH, clk >> 8);
    writel(SER_BASE + UART8250_LCR, UART8250_LCR_WLS_8);
}
