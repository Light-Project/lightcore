/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_H_
#define _DRIVER_IRQCHIP_H_ 

#include <types.h>
#include <state.h>

#include <asm/irq.h>

struct irqchip_device;

struct irqchip_ops {
    void (*ack)(struct irqchip_device *, irqnr_t irq);
    void (*pass)(struct irqchip_device *, irqnr_t irq);
    void (*mask)(struct irqchip_device *, irqnr_t irq);
    void (*eoi)(struct irqchip_device *, irqnr_t irq);
};

struct irqchip_device {
    struct irqchip_ops *ops;

    uint32_t mask;
    void *data;

    struct device *device;
};

state irqchip_pass(irqnr_t vector);

state irqchip_register(struct irqchip_device *);
void irqchip_init(void);

#endif  /* _DRIVER_IRQCHIP_H_ */
