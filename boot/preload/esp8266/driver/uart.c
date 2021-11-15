/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/uart/esp8266.h>
#include <asm/proc.h>
#include <asm/io.h>

void uart_putc(char byte)
{
    uint32_t val;

    for (;;) {
        val = readl(UART_BASE + ESP8266_UART_STATUS);
        if (((val >> 16) & 0xff) == 0)
            break;
        cpu_relax();
    }

    writel(UART_BASE + ESP8266_UART_FIFO, byte);
}

void uart_print(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');
        uart_putc(*str++);
    }
}

void uart_init(void)
{
    uint32_t val;

    uart_deinit();

    val = ESP8266_UART_CONF0_STB_1 | ESP8266_UART_CONF0_WSL_8;
    writel(UART_BASE + ESP8266_UART_CONF0, val);

    val = (APB_FREQ / UART_BAUD) & ESP8266_UART_CLKDIV_DIV;
    writel(UART_BASE + ESP8266_UART_CLKDIV, val);

    val = readl(UART_BASE + ESP8266_UART_CONF0);
    val |= ESP8266_UART_CONF0_TXFIFO_RST;
    writel(UART_BASE + ESP8266_UART_CONF0, val);

    val = readl(UART_BASE + ESP8266_UART_CONF0);
    val &= ~ESP8266_UART_CONF0_TXFIFO_RST;
    writel(UART_BASE + ESP8266_UART_CONF0, val);

}

void uart_deinit(void)
{
    writel(UART_BASE + ESP8266_UART_CLKDIV, ESP8266_UART_CLKDIV_DEFAULT);
    writel(UART_BASE + ESP8266_UART_INT_ENA, ESP8266_UART_INT_ENA_DEFAULT);
    writel(UART_BASE + ESP8266_UART_AUTOBAUD, ESP8266_UART_AUTOBAUD_DEFAULT);
    writel(UART_BASE + ESP8266_UART_CONF0, ESP8266_UART_CONF0_DEFAULT);
    writel(UART_BASE + ESP8266_UART_CONF1, ESP8266_UART_CONF1_DEFAULT);
    writel(UART_BASE + ESP8266_UART_LOWPULSE, ESP8266_UART_LOWPULSE_DEFAULT);
    writel(UART_BASE + ESP8266_UART_HIGHPULSE, ESP8266_UART_HIGHPULSE_DEFAULT);
    writel(UART_BASE + ESP8266_UART_RXDCNT, ESP8266_UART_RXDCNT_DEFAULT);
}
