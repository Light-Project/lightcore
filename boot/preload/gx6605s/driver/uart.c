/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "boot.h"
#include <driver/platform/gx6605s.h>
#include <driver/uart/gx6605s.h>
#include <asm/proc.h>
#include <asm/io.h>

#if 1

void uart_putc(char ch)
{
    while (!(readl(SER_BASE + GX6605S_UART_STA) & GX6605S_UART_STA_THRE))
        cpu_relax();
    writel(SER_BASE + GX6605S_UART_DAT, ch);
}

void uart_print(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');
        uart_putc(*str++);
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

#else

#include <driver/uart/8250.h>

#undef SER_BASE
#define SER_BASE ((void *)0x403000)

void uart_putc(char ch)
{
    while (!(readl(SER_BASE + UART8250_LSR) & UART8250_LSR_THRE))
        cpu_relax();
    writel(SER_BASE + UART8250_THR, ch);
}

void uart_print(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');
        uart_putc(*str++);
    }
}

void uart_init(uint32_t apb, uint32_t freq)
{
    uint32_t clk = (apb / freq) / 16;
    uint32_t val;

    val = readl(GCTL_BASE + GX6605S_PINMUX_PORTA);
    val &= ~(BIT(16) | BIT(17));
    val |= BIT(16);
    writel(GCTL_BASE + GX6605S_PINMUX_PORTA, val);
    val = readl(GCTL_BASE + GX6605S_PINMUX_PORTB);
    val &= ~(BIT(9) | BIT(10));
    val |= BIT(9);
    writel(GCTL_BASE + GX6605S_PINMUX_PORTB, val);

    writel(SER_BASE + UART8250_IER, 0);

    writel(SER_BASE + UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    writel(SER_BASE + UART8250_FCR, UART8250_FCR_FIFO_EN);

    writel(SER_BASE + UART8250_LCR, UART8250_LCR_DLAB);
    writel(SER_BASE + UART8250_DLL, clk);
    writel(SER_BASE + UART8250_DLH, clk >> 8);
    writel(SER_BASE + UART8250_LCR, UART8250_LCR_WLS_8);
}

#endif
