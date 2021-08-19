/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"dt: " fmt

#include <driver/dt.h>
#include <driver/dt/libfdt.h>

static __init void fdt_address_cell(const void *blob, int parentoffset,
                                   int *addrc, int *sizec)
{
	const be32 *prop;

    if (addrc) {
        prop = fdt_getprop(blob, parentoffset, "#address-cells", NULL);
        if (prop)
            *addrc = be32_to_cpup(prop);
    }

    if (sizec) {
        prop = fdt_getprop(blob, parentoffset, "#size-cells", NULL);
        if (prop)
            *sizec = be32_to_cpup(prop);
    }
}

__init void *fdt_address(const void *blob, unsigned int node)
{
	int parent, len;
	const be32 *reg;
    be32 addr[4];
	int addrc, sizec;

    reg = fdt_getprop(blob, node, "reg", &len);
	if (!reg)
        return NULL;

	parent = fdt_parent_offset(blob, node);
	if (parent < 0)
        return NULL;

	fdt_address_cell(blob, parent, &addrc, &sizec);

    memcpy(addr, reg, addrc * 4);

    return NULL;
}
