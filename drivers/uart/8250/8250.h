#ifndef _UART8250_H_
#define _UART8250_H_

#include <bits.h>
#include <kernel.h>
#include <driver/uart.h>

enum uart8250_flags {
    UART8250_FIFO   = BIT(0),
};

struct uart8250_device {
    enum uart8250_flags flags;
    struct uart_device uart;
    struct uart8250_ops *ops;
};

struct uart8250_ops {
    uint32_t (*port_in)(struct uart8250_device *, uint8_t);
    void (*port_out)(struct uart8250_device *, uint8_t, uint32_t);
};

#define udev_to_uart8250(udev) \
    container_of(udev, struct uart8250_device, uart);

state uart8250_register(struct uart8250_device *);
void uart8250_unregister(struct uart8250_device *);

#endif  /* _UART8250_H_ */
