/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_H_
#define _DRIVER_IRQCHIP_H_

#include <device.h>
#include <driver/dt.h>

extern struct list_head irqchip_list;

struct irqchip_channel {
    struct irqchip_device *irqchip;
    struct list_head list;
    unsigned long index;
};

struct irqchip_device {
    struct device *dev;
    struct list_head list;
    struct irqchip_ops *ops;
    struct dt_node *dt_node;

    struct list_head channel;
    struct list_head child;
    struct list_head sibling;
    irqnr_t channel_max;
};

struct irqchip_ops {
    state (*pass)(struct irqchip_device *, irqnr_t);
    state (*mask)(struct irqchip_device *, irqnr_t);
    state (*ack)(struct irqchip_device *, irqnr_t);
    state (*eoi)(struct irqchip_device *, irqnr_t);
    state (*slave_add)(struct irqchip_device *, irqnr_t);
    state (*slave_del)(struct irqchip_device *, irqnr_t);
};

struct irqchip_channel *dt_get_irqchip_channel(struct dt_node *node, int index);
struct irqchip_channel *dt_get_irqchip_channel_by_name(struct dt_node *node, const char *name);

state irqchip_pass(struct irqchip_channel *);
state irqchip_mask(struct irqchip_channel *);
state irqchip_ack(struct irqchip_channel *);
state irqchip_eoi(struct irqchip_channel *);

struct irqchip_channel *irqchip_channel_get(struct irqchip_device *idev, irqnr_t chnr);
void irqchip_channel_release(struct irqchip_channel *channel);
state irqchip_register(struct irqchip_device *);
void irqchip_unregister(struct irqchip_device *);
void irqchip_init(void);

#endif  /* _DRIVER_IRQCHIP_H_ */
