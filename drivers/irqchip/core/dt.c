/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/irqchip.h>

static struct dt_node *dt_irq_parent(struct dt_node *node)
{
    uint32_t phandle;

    if (!dt_search_up(node, "interrupt-parent", &phandle))
        return NULL;

    return dt_find_by_phandle(phandle);
}

static struct dt_node *dt_get_channel(struct dt_node *node, int index, uint32_t *irq_nr)
{
    struct dt_node *parent;
    uint32_t int_cell;
    // unsigned int c;

    /* Get the irq parent */
    parent = dt_irq_parent(node);
    if (!parent)
        return NULL;

    if (dt_attribute_read_u32(parent, "#interrupt-cells", &int_cell))
        return NULL;

    // for (c = 0; c < int_cell; c++) {
        dt_attribute_read_u32_index(node, "interrupts", index, irq_nr);
    // }

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
 * @node:
 * @index:
 */
struct irqchip_channel *dt_get_irqchip_channel(struct dt_node *node, int index)
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

struct irqchip_channel *dt_get_irqchip_channel_by_name(struct dt_node *node, const char *name)
{
    int index;

    if (!node || !name)
        return NULL;

    index = dt_attribute_string_index(node, "interrupt-names", name);
    if (index < 0)
        return NULL;

    return dt_get_irqchip_channel(node, index);
}
