/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/dt/fdt.h>
#include <export.h>

state dt_address(const struct dt_node *node, unsigned int index,
                 resource_size_t *addr, resource_size_t *size)
{
    const be32 *prop;
    uint32_t acell, scell, cell;
    int len;

    acell = dt_addr_cell(node);
    scell = dt_size_cell(node);
    cell = acell + scell;

    prop = dt_attribute_get(node, "reg", &len);
    len /= 4;

    if (!prop || len < ((index + 1) * cell))
        return -ENODATA;

    *addr = (resource_size_t)dt_read(prop + (index * cell), acell);
    *size = (resource_size_t)dt_read(prop + (index * cell) + acell, scell);

    return -ENOERR;
}
EXPORT_SYMBOL(dt_address);

unsigned int dt_address_numbers(const struct dt_node *node)
{
    uint32_t acell, scell, cell;
    int len;

    acell = dt_addr_cell(node) * 4;
    scell = dt_size_cell(node) * 4;
    cell = acell + scell;

    if (!dt_attribute_get(node, "reg", &len))
        return 0;

    return len / cell;
}
EXPORT_SYMBOL(dt_address_numbers);
