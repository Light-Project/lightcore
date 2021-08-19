/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/dt/fdt.h>

state dt_address(const struct dt_node *node, int index,
                 resource_size_t *addr, resource_size_t *size)
{
    const be32 *prop;
    int len, acell, scell;

    acell = dt_addr_cell(node);
    scell = dt_size_cell(node);

    prop = dt_attribute_get(node, "reg", &len);
    if (!prop || len < (index * (acell + scell)))
        return -ENODATA;

    *addr = dt_read(prop, acell);
    *size = dt_read(prop + acell, scell);
    return -ENOERR;
}

int dt_address_nr(const struct dt_node *node)
{
    int len, asize, sszie;

    asize = dt_addr_cell(node) * 4;
    sszie = dt_size_cell(node) * 4;

    if (!dt_attribute_get(node, "reg", &len))
        return -ENODATA;

    return len / (asize + sszie);
}
