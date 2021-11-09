/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/dt/fdt.h>
#include <export.h>

state dt_irq(const struct dt_node *node, unsigned int index,
             resource_size_t *irq)
{

    return -ENOERR;
}
EXPORT_SYMBOL(dt_irq);

unsigned int dt_irq_nr(const struct dt_node *node)
{
    int len, asize, sszie;

    asize = dt_addr_cell(node) * 4;
    sszie = dt_size_cell(node) * 4;

    if (!dt_attribute_get(node, "irq", &len))
        return -ENODATA;

    return len / (asize + sszie);
}
EXPORT_SYMBOL(dt_irq_nr);
