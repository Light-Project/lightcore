/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <ioops.h>
#include <proc.h>
#include <driver/uart/8250.h>

#define I8250_BASE  0x3f8
#define I8250_FREQ  115200

void serial_print(const char *str)
{
    while (*str) {
        while (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_THRE))
            cpu_relax();
        outb(I8250_BASE + UART8250_THR, *str++);
    }
}

void serial_init(void)
{
    uint16_t div = 115200 / I8250_FREQ;

    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_DLAB);
    outb(I8250_BASE + UART8250_DLL, div);
    outb(I8250_BASE + UART8250_DLH, div >> 8);

    outb(I8250_BASE + UART8250_IER, 0x00);
    outb(I8250_BASE + UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    outb(I8250_BASE + UART8250_FCR, UART8250_FCR_FIFO_EN);
    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_WLS_8);
}
