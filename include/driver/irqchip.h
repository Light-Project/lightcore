/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_H_
#define _DRIVER_IRQCHIP_H_

#include <device.h>
#include <driver/dt.h>

/**
 * struct irqchip_channel - Describe the channel of an interrupt controller.
 * @irqchip: the parent interrupt controller for the channel.
 * @list: the channel is inserted on the @channel of the parent interrupt controller.
 * @index: channel number.
 */
struct irqchip_channel {
    struct irqchip_device *irqchip;
    struct list_head list;
    unsigned long index;
};

/**
 * struct irqchip_device - Describe an interrupt controller.
 * @dev: points to the parent device of the interrupt controller.
 * @list: linked list for centralized management of interrupt controllers.
 * @ops: operations method of interrupt controller.
 * @dt_node: device tree node pointing to the interrupt controller.
 * @channel: linked list of allocated channels for the interrupt controller.
 * @child: subinterrupt controllers owned by an interrupt controller.
 * @sibling: the child interrupt controller is plugged into the node on @child.
 * @channel_nr: number of channels owned by the interrupt controller.
 */
struct irqchip_device {
    struct device *dev;
    struct list_head list;
    struct irqchip_ops *ops;
    struct dt_node *dt_node;
    struct list_head channel;
    struct list_head child;
    struct list_head sibling;
    irqnr_t channel_nr;
};

/**
 * struct irqchip_ops - Describe the operations of a controller.
 * @pass: pass the interrupt gate of a channel.
 * @mask: mask the interrupt gate of a channel.
 * @ack: manually trigger interrupt of a channel.
 * @eoi: end the interrupt of a channel.
 * @slave_add: add a child interrupt controller to the interrupt controller.
 * @slave_del: remove a child interrupt controller from an interrupt controller.
 */
struct irqchip_ops {
    state (*pass)(struct irqchip_device *idev, irqnr_t channel);
    state (*mask)(struct irqchip_device *idev, irqnr_t channel);
    state (*ack)(struct irqchip_device *idev, irqnr_t channel);
    state (*eoi)(struct irqchip_device *idev, irqnr_t channel);
    state (*slave_add)(struct irqchip_device *idev, irqnr_t channel);
    state (*slave_del)(struct irqchip_device *idev, irqnr_t channel);
};

extern struct list_head irqchip_list;

/* irqchip operations */
extern state irqchip_pass(struct irqchip_channel *idev);
extern state irqchip_mask(struct irqchip_channel *idev);
extern state irqchip_ack(struct irqchip_channel *idev);
extern state irqchip_eoi(struct irqchip_channel *idev);
extern state irqchip_slave_add(struct irqchip_channel *channel, struct irqchip_device *slave);
extern state irqchip_slave_del(struct irqchip_channel *channel, struct irqchip_device *slave);

/* irqchip device tree linker */
#ifdef CONFIG_IRQCHIP_DT
extern struct irqchip_channel *dt_irqchip_channel(struct dt_node *node, int index);
extern struct irqchip_channel *dt_irqchip_channel_name(struct dt_node *node, const char *name);
extern void __init dt_irqchip_slave_setup(void);
#endif

/* irqchip channel core */
extern struct irqchip_channel *irqchip_channel_get(struct irqchip_device *idev, irqnr_t chnr);
extern void irqchip_channel_release(struct irqchip_channel *channel);
extern state irqchip_register(struct irqchip_device *);
extern void irqchip_unregister(struct irqchip_device *);
extern void __init irqchip_init(void);

#endif /* _DRIVER_IRQCHIP_H_ */
