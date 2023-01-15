/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <console.h>
#include <initcall.h>
#include <ioops.h>
#include <driver/uart/8250.h>

#define I8250_BASE CONFIG_EARLYCON_SERIAL_BASE
#define I8250_FREQ CONFIG_EARLYCON_SERIAL_FREQ

static unsigned int serial_read(struct console *con, char *str, unsigned int len)
{
    unsigned int rlen;

    for (rlen = 0; rlen < len; ++rlen) {
        if (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_DR))
            break;
        *str++ = inb(I8250_BASE + UART8250_THR);
    }

    return rlen;
}

static void serial_write(struct console *con, const char *str, unsigned int len)
{
    while (len--) {
        while (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_THRE))
            cpu_relax();
        outb(I8250_BASE + UART8250_THR, *str++);
    }
}

static void serial_sync(struct console *con)
{
    while (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_THRE))
        cpu_relax();
    outb(I8250_BASE + UART8250_FCR, UART8250_FCR_CLEAR_RCVR);
}

static struct console_ops ser_console_ops = {
    .read = serial_read,
    .write = serial_write,
    .sync = serial_sync,
};

static struct console ser_console = {
    .name = "early-serial",
    .ops = &ser_console_ops,
    .flags = CONSOLE_BOOT,
};

static void serial_hwinit(void)
{
    static bool already;
    uint16_t div = 115200 / I8250_FREQ;

    if (already)
        return;
    already = true;

    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_DLAB);
    outb(I8250_BASE + UART8250_DLL, div);
    outb(I8250_BASE + UART8250_DLH, div >> 8);

    outb(I8250_BASE + UART8250_IER, 0x00);
    outb(I8250_BASE + UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    outb(I8250_BASE + UART8250_FCR, UART8250_FCR_FIFO_EN);
    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_WLS_8);
}

static state serial_console_init(void)
{
    serial_hwinit();
    console_register(&ser_console);
    return -ENOERR;
}
console_initcall(serial_console_init);

#ifdef CONFIG_EARLYCON_SERIAL
void earlycon_init(void)
{
    serial_hwinit();
    earlycon_register(&ser_console);
}
#endif
