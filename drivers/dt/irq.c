/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/dt/fdt.h>
#include <export.h>

unsigned int dt_irq_nr(const struct dt_node *node)
{
    struct dt_node *parent;
    uint32_t cell;
    int len;

    parent = dt_irq_parent(node);
    if (!parent)
        return 0;

    if (dt_attribute_read_u32(parent, "#interrupt-cells", &cell))
        return 0;

    if (!dt_attribute_get(node, "irq", &len))
        return 0;

    return len / cell;
}
EXPORT_SYMBOL(dt_irq_nr);

struct dt_node *dt_irq_parent(const struct dt_node *node)
{
    uint32_t phandle;

    if (!dt_search_up(node, "interrupt-parent", &phandle))
        return NULL;

    return dt_find_by_phandle(phandle);
}
EXPORT_SYMBOL(dt_irq_parent);

state dt_irq_parse(const struct dt_node *node, unsigned int index,
             resource_size_t *irq)
{

    return -ENOERR;
}
EXPORT_SYMBOL(dt_irq_parse);
