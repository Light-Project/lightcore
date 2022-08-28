/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "boot.h"
#include <driver/platform/gx6605s.h>
#include <driver/uart/gx6605s.h>
#include <asm/proc.h>
#include <asm/io.h>

void uart_print(const char *str)
{
    while (*str) {
        while (!(readl(SER_BASE + GX6605S_UART_STA) & GX6605S_UART_STA_THRE))
            cpu_relax();
        writel(SER_BASE + GX6605S_UART_DAT, ch);
    }
}

void uart_init(uint32_t apb, uint32_t freq)
{
    uint32_t clkdiv, val;

    val = readl(GCTL_BASE + GX6605S_PINMUX_PORTA);
    val &= ~(BIT(16) | BIT(17));
    writel(GCTL_BASE + GX6605S_PINMUX_PORTA, val);
    val = readl(GCTL_BASE + GX6605S_PINMUX_PORTB);
    val &= ~(BIT(9) | BIT(10));
    writel(GCTL_BASE + GX6605S_PINMUX_PORTB, val);

    clkdiv = apb / ((16 * freq) / 100);
    if (clkdiv % 100 > 50)
        clkdiv = clkdiv / 100;
    else
        clkdiv = clkdiv / 100 - 1;

    writel(SER_BASE + GX6605S_UART_CLK, clkdiv);
    writel(SER_BASE + GX6605S_UART_CTL, 0x600);
}
