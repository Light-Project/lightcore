/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <stddef.h>
#include <state.h>
#include <asm/io.h>
#include <driver/uart/uart-esp.h>

#include <boot.h>

#define UART_BASE 0x60000000
#define UART_BAUD 115200

void uart_putc(char byte)
{
    uint32_t status;
    
    for(;;) {
        status = readl_sync((void *)UART_BASE + UART_ESP_STATUS);
        if(((status >> 16) & 0xff) == 0)
            break;
    }
    
    writel_sync((void *)UART_BASE + UART_ESP_FIFO, byte);
}

void uart_print(const char *str)
{
    if(str == NULL)
        return;
    while(*str != '\0')
        uart_putc(*str++);
}

void uart_init(void)
{
    uint32_t val;

    uart_deinit();

    /* 8n1 */
    val = UART_ESP_CONF0_STB_1 | UART_ESP_CONF0_WSL_8;
    writel_sync((void *)UART_BASE + UART_ESP_CONF0, val);

    /* set baudrate */
    val = (UART_CLK_FREQ / UART_BAUD) & UART_ESP_CLKDIV_DIV;
    writel_sync((void *)UART_BASE + UART_ESP_CLKDIV, val);

    val = readl_sync((void *)UART_BASE + UART_ESP_CONF0);
    val |= UART_ESP_CONF0_TXFIFO_RST;
    writel_sync((void *)UART_BASE + UART_ESP_CONF0, val);

    val = readl_sync((void *)UART_BASE + UART_ESP_CONF0);
    val &= ~UART_ESP_CONF0_TXFIFO_RST;
    writel_sync((void *)UART_BASE + UART_ESP_CONF0, val);

}

void uart_deinit(void)
{
    writel((void *)UART_BASE + UART_ESP_CLKDIV, UART_ESP_CLKDIV_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_INT_ENA, UART_ESP_INT_ENA_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_AUTOBAUD, UART_ESP_AUTOBAUD_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_CONF0, UART_ESP_CONF0_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_CONF1, UART_ESP_CONF1_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_LOWPULSE, UART_ESP_LOWPULSE_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_HIGHPULSE, UART_ESP_HIGHPULSE_DEFAULT);
    writel((void *)UART_BASE + UART_ESP_RXDCNT, UART_ESP_RXDCNT_DEFAULT);
}
