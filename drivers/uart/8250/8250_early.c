#include <stddef.h>     // for DIV_ROUND_CLOSEST
#include <asm/io.h>
#include <asm/proc.h>
#include <driver/uart.h>
#include <driver/uart/uartcon.h>
#include <driver/uart/8250.h>

static unsigned int uart8250_early_in(struct uart_port *port, int offset)
{
    int reg_offset = offset;
    offset <<= port->regshift;

    switch (port->iotype) {
    case UPIO_MEM:
        return readb(port->membase + offset);
    case UPIO_MEM16:
        return readw(port->membase + offset);
    case UPIO_MEM32:
        return readl(port->membase + offset);
    case UPIO_PORT:
        return inb(port->iobase + offset);
    case UPIO_AU:
        return port->read(port, reg_offset);
    default:
        return 0;
    }
}

static void uart8250_early_out(struct uart_port *port, int offset, int value)
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

static void uart8250_early_putc(struct uart_port *port, const char c)
{
    while(uart8250_early_in(port, UART8250_LSR) & both_empty == both_empty)
        cpu_relax();
    uart8250_early_out(port, UART8250_RBR, c);
}

static void uart8250_early_read(struct console *console, const char *str, unsigned int chount)
{
    
}

static void uart8250_early_write(struct console *console, const char *str, unsigned int chount)
{
    struct uart_port port = console->data;
    
    while(chount--)
        uart8250_early_putc(port, *str++);
}

static void uart8250_early_init(struct uartcon_device *device)
{
    unsigned int div;
    size_t proc;
    
    struct uart_port *port = &device->port;
    
    uart8250_early_out(port, UART8250_LCR, 0x03);   /* 8n1          */
    uart8250_early_out(port, UART8250_IER, 0x00);   /* no interrupt */
    uart8250_early_out(port, UART8250_FCR, 0x00);   /* no fifo      */
    uart8250_early_out(port, UART8250_MCR, 0x00);   /* Clean MCR    */
    
    if(port->uartclk)
    {
        div = DIV_ROUND_CLOSEST(port->uartclk, 16 * device->baud);
        proc = uart8250_early_in(port, UART8250_LCR);
        uart8250_early_out(port, UART8250_LCR, proc | UART8250_LCR_DLAB);   /* set DLAB */
        uart8250_early_out(port, UART8250_DLR_LSB, div & 0xff);
        uart8250_early_out(port, UART8250_DLR_MSB, (div >> 8) & 0xff);
        proc = uart8250_early_in(port, UART8250_LCR);
        uart8250_early_out(port, UART8250_LCR, proc & ~UART8250_LCR_DLAB);  /* clr DLAB */
    }
}

void uart8250_early_setup(void)
{


}

DT_EARLYCON_DECLARE(ns16550, "ns16550", uart8250_early_init);
DT_EARLYCON_DECLARE(ns16550, "ns16550a", early_serial8250_setup);
