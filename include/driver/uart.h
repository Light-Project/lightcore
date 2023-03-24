/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_UART_H_
#define _DRIVER_UART_H_

#include <state.h>
#include <spinlock.h>
#include <lightcore/uart.h>

struct uart_device {
    const char *name;
    spinlock_t lock;
    struct uart_ops *ops;
};

struct uart_ops {
	void (*start_tx)(struct uart_device *);
	void (*stop_tx)(struct uart_device *);
    void (*putc)(struct uart_device *, char ch);

    state (*startup)(struct uart_device *);
    void  (*shutdown)(struct uart_device *);
    state (*ioctl)(struct uart_device *, unsigned int, size_t);
    void  (*sync)(struct uart_device *);
};

state uart_register(struct uart_device *);
void uart_unregister(struct uart_device *);

#endif /* _DRIVER_UART_H_ */
