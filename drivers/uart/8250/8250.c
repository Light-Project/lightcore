/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "8250.h"
#include <driver/uart/8250.h>
#include <asm/proc.h>
#include <asm/io.h>

static __always_inline uint32_t
uart8250_in(struct uart8250_device *uart8250, uint8_t port)
{
    struct uart8250_ops *ops = uart8250->ops;
    return ops->port_in(uart8250, port);
}

static __always_inline void
uart8250_out(struct uart8250_device *uart8250, uint8_t port, uint32_t val)
{
    struct uart8250_ops *ops = uart8250->ops;
    return ops->port_out(uart8250, port, val);
}

static void uart8250_fifos_clear(struct uart8250_device *uart8250)
{
    if (!(uart8250->flags & UART8250_FIFO))
        return;

    uart8250_out(uart8250, UART8250_FCR, UART8250_FCR_FIFO_EN);
    uart8250_out(uart8250, UART8250_FCR, UART8250_FCR_FIFO_EN |
                 UART8250_FCR_CLEAR_RCVR | UART8250_FCR_CLEAR_XMIT);
    uart8250_out(uart8250, UART8250_FCR, 0);
}

static state uart8250_startup(struct uart_device *udev)
{
    struct uart8250_device *uart8250 = udev_to_uart8250(udev);

    uart8250_fifos_clear(uart8250);

    return -ENOERR;
}

static void uart8250_shutdown(struct uart_device *udev)
{
    struct uart8250_device *uart8250 = udev_to_uart8250(udev);
    uint32_t val;

    val = uart8250_in(uart8250, UART8250_IER);
    uart8250_out(uart8250, UART8250_IER, val & ~UART8250_LCR_SBRK);

    uart8250_fifos_clear(uart8250);
}

static struct uart_ops uart8250_ops = {
    .startup = uart8250_startup,
    .shutdown = uart8250_shutdown,
};

state uart8250_register(struct uart8250_device *uart8250)
{
    uart8250->uart.ops = &uart8250_ops;
    return uart_register(&uart8250->uart);
}

void uart8250_unregister(struct uart8250_device *uart8250)
{
    uart_unregister(&uart8250->uart);
}
