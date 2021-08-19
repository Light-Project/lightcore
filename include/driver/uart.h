/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_UART_H_
#define _DRIVER_UART_H_

#include <kernel/spinlock.h>
#include <lightcore/uart.h>

struct uart_device {
    const char *name;   /* port name */
    spinlock_t lock;    /* port lock */

    state (*startup)(struct uart_device *);
    state (*shutdown)(struct uart_device *);
};

struct uart_ops {
	void (*start_tx)(struct uart_device *);
	void (*stop_tx)(struct uart_device *);
	void (*stop_rx)(struct uart_device *);
    void (*sent_char)(struct uart_device *, char ch);
    void (*startup)(struct uart_device *);
    void (*shutdown)(struct uart_device *);
    void (*sync)(struct uart_device *);
};

int uart_register(struct uart_device *);

#endif  /* _DRIVER_UART_H_ */
