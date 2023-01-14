/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/uart.h>
#include <driver/uart/uart-esp.h>
#include <asm/io.h>

#define ESP_UART_MAX    3

static void esp_uart_putc(struct uart_port *port, char ch)
{
    uint32_t status;
    for(;;)
    {
        status = readl((void *)(port->membase + UART_ESP_STATUS));
        if(((status >> 16) & 0xff) == 0)
            break;
    }
    writel((void *)(port->membase + UART_ESP_FIFO), ch);
}

static void esp_uart_console_write(struct console *con, const char *str,
                    unsigned int len)
{
    struct uart_port *port = con->data;
    irqflags_t irqflags;
    uint32_t ena_old;

    spin_lock_irqsave(&port->lock, &irqflags);

    /* Save and disable interrupt */
    ena_old = readl((void *)port->membase + UART_ESP_INT_ENA);
    writel((void *)port->membase + UART_ESP_INT_ENA, UART_ESP_INT_ENA_DEFAULT);

    while(len--)
        esp_uart_putc(port, *str++);

    /* Recovery register */
    writel((void *)port->membase + UART_ESP_INT_ENA, ena_old);

    spin_unlock_irqrestore(&port->lock, &irqflags);
}

static int esp_uart_console_setup(struct console *con, char *options)
{
//     struct uart_port *port;
//     int baud = 115200;
//     int bits = 8;
//     int parity = 'n';
//     int flow = 'n';


    return 0;
}

static struct console esp_uart_console = {
    .name   = ESP_UART_NAME,
    .setup  = esp_uart_console_setup,
    .write  = esp_uart_console_write,
};

#include <printk.h>

static state __init esp_uart_console_init()
{
    console_register(&esp_uart_console);
    return -ENOERR;
}

console_initcall(esp_uart_console_init)

static void esp_uart_startup(struct uart_port *port)
{
    uint32_t val;

    /* 8n1 */
    val = UART_ESP_CONF0_STB_1 | UART_ESP_CONF0_WSL_8;
    writel(port->membase + UART_ESP_CONF0, val);

    // /* set baudrate */
    // val = (UART_CLK_FREQ / UART_BAUD) & UART_ESP_CLKDIV_DIV;
    // writel(port->membase + UART_ESP_CLKDIV, val);
}

static void esp_uart_shutdown(struct uart_port *port)
{
    writel(port->membase + UART_ESP_CLKDIV, UART_ESP_CLKDIV_DEFAULT);
    writel(port->membase + UART_ESP_INT_ENA, UART_ESP_INT_ENA_DEFAULT);
    writel(port->membase + UART_ESP_AUTOBAUD, UART_ESP_AUTOBAUD_DEFAULT);
    writel(port->membase + UART_ESP_CONF0, UART_ESP_CONF0_DEFAULT);
    writel(port->membase + UART_ESP_CONF1, UART_ESP_CONF1_DEFAULT);
    writel(port->membase + UART_ESP_LOWPULSE, UART_ESP_LOWPULSE_DEFAULT);
    writel(port->membase + UART_ESP_HIGHPULSE, UART_ESP_HIGHPULSE_DEFAULT);
    writel(port->membase + UART_ESP_RXDCNT, UART_ESP_RXDCNT_DEFAULT);
}

static struct uart_ops esp_uart_ops = {
    .startup = esp_uart_startup,
    .shutdown = esp_uart_shutdown,
    .sent_char = esp_uart_putc,
};

static state espressif_uart_probe(struct platform_device *device, const void *pdata)
{
    struct esp_uart_port *esp_port;
    struct uart_port *port;

    esp_port = kzalloc(sizeof(*esp_port), GFP_KERNEL);
    if (!esp_port)
        return -ENOMEM;

    port = &esp_port->port;
    port->ops = &esp_uart_ops;

    uart_register(&esp_uart_driver, &esp_port->port);
    return -ENOERR;
}

static void espressif_uart_remove(struct platform_device *device)
{
    struct esp_uart_port* esp_port;
    esp_port = (struct esp_uart_port *)platform_get_devdata(device);

    kfree(esp_port);

    return -ENOERR;
}

static struct dt_device_id espressif_uart_dt_table[] = {
    { .compatible = "espressif,esp8266-uart" },
    { }, /* NULL */
};

static struct platform_driver espressif_uart_driver = {
    .driver = {
        .name = "espressif-uart",
    },
    .dt_table = espressif_uart_dt_table,
    .probe  = espressif_uart_probe,
    .remove = espressif_uart_remove,
};

static state espressif_uart_init(void)
{
    return platform_driver_register(&espressif_uart_driver);
}
driver_initcall(espressif_uart_init);
