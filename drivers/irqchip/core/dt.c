/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/irqchip.h>
#include <export.h>

static struct dt_node *dt_get_channel(struct dt_node *node, int index, uint32_t *irq_nr)
{
    struct dt_node *parent;
    uint32_t int_cell;

    /* Get the irq parent */
    parent = dt_irq_parent(node);
    if (!parent)
        return NULL;

    if (dt_attribute_read_u32(parent, "#interrupt-cells", &int_cell))
        return NULL;

    dt_attribute_read_u32_index(node, "interrupts", index, irq_nr);

    return parent;
}

static struct irqchip_device *dt_irqchip_find(struct dt_node *node)
{
    struct irqchip_device *idev;

    list_for_each_entry(idev, &irqchip_list, list)
        if (idev->dt_node == node)
            return idev;

    return NULL;
}

/**
 * dt_get_irq - Get interrupt number form device tree
 * @node: device tree node
 * @index: irq number index
 */
struct irqchip_channel *dt_irqchip_channel(struct dt_node *node, int index)
{
    struct irqchip_device *idev;
    struct dt_node *parent;
    uint32_t chnr;

    parent = dt_get_channel(node, index, &chnr);
    if (!parent)
        return NULL;

    idev = dt_irqchip_find(parent);
    if (!idev)
        return NULL;

    return irqchip_channel_get(idev, chnr);
}
EXPORT_SYMBOL(dt_irqchip_channel);

/**
 * dt_get_irq - Get interrupt number form device tree
 * @node: device tree node
 * @name: irq number name
 */
struct irqchip_channel *dt_irqchip_channel_name(struct dt_node *node, const char *name)
{
    int index;

    if (!node || !name)
        return NULL;

    index = dt_attribute_string_index(node, "interrupt-names", name);
    if (index < 0)
        return NULL;

    return dt_irqchip_channel(node, index);
}
EXPORT_SYMBOL(dt_irqchip_channel_name);

void __init dt_irqchip_slave_setup(void)
{
    struct irqchip_device *idev;
    struct irqchip_channel *channel;

    list_for_each_entry(idev, &irqchip_list, list) {
        if (!(channel = dt_irqchip_channel(idev->dt_node, 0)))
            continue;
        irqchip_slave_add(channel, idev);
    }
}
