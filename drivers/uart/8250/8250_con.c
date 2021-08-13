/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <init/initcall.h>
#include <console.h>
#include <driver/uart.h>
#include <driver/uart/8250.h>

#include <asm/io.h>
#include <asm/proc.h>

static unsigned int i8250_in(struct uart_port *port, int reg)
{
    reg <<= port->regshift;

    switch (port->iotype) {
    case UPIO_MEM:
        return readb(port->membase + reg);
    case UPIO_MEM16:
        return readw(port->membase + reg);
    case UPIO_MEM32:
        return readl(port->membase + reg);
    case UPIO_PORT:
        return inb(port->iobase + reg);
    default:
        return 0;
    }
}

static void i8250_out(struct uart_port *port, int offset, unsigned int value)
{
    offset <<= port->regshift;

    switch (port->iotype) {
    case UPIO_MEM:
        writeb((void *)port->membase + offset, value);
        break;
    case UPIO_MEM16:
        writew((void *)port->membase + offset, value);
        break;
    case UPIO_MEM32:
        writel((void *)port->membase + offset, value);
        break;
    case UPIO_PORT:
        outb((unsigned int)port->membase + offset, value);
        break;
    case UPIO_AU:
        port->write(port->membase + offset, value);
        break;
    }
} 

#define both_empty (UART8250_LSR_THRE | UART8250_LSR_TEMT)

static inline void i8250_putc(struct uart_port *port, char ch)
{
    while(i8250_in(port, UART8250_LSR) & both_empty == both_empty)
        cpu_relax();
    i8250_out(port, UART8250_RBR, ch);
}

static void i8250_write(struct console *console, const char *str, unsigned int count)
{
    struct uart_port port = console->data;
    while(count--)
        uart8250_early_putc(port, *str++);
}

static void i8250_init(struct uartcon_device *device)
{
    unsigned int div;
    size_t proc;
    
    struct uart_port *port = &device->port;
    
    i8250_out(port, UART8250_LCR, 0x03);   /* 8n1          */
    i8250_out(port, UART8250_IER, 0x00);   /* no interrupt */
    i8250_out(port, UART8250_FCR, 0x00);   /* no fifo      */
    i8250_out(port, UART8250_MCR, 0x00);   /* Clean MCR    */
    
    if(port->uartclk) {
        div = DIV_ROUND_CLOSEST(port->uartclk, 16 * device->baud);
        proc = i8250_in(port, UART8250_LCR);
        i8250_out(port, UART8250_LCR, proc | UART8250_LCR_DLAB);   /* set DLAB */
        i8250_out(port, UART8250_DLR_LSB, div & 0xff);
        i8250_out(port, UART8250_DLR_MSB, (div >> 8) & 0xff);
        proc = i8250_in(port, UART8250_LCR);
        i8250_out(port, UART8250_LCR, proc & ~UART8250_LCR_DLAB);  /* clr DLAB */
    }
}
console_initcall(uart8250_early_setup);
