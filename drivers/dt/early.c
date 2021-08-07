/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <state.h> 
#include <crc.h>
#include <mm/memblock.h>
#include <init/init.h>
#include <driver/dt/fdt.h>
#include <driver/dt/libfdt.h>
#include <printk.h>

static state __init dt_scan_chosen(unsigned long node, const char *uname, int depth, void *data)
{
    int len;
    const char *args;
    const void *rng_seed;

    /* Matching nodes */
    if (depth != 1 || !data ||
        (strcmp(uname, "chosen") != 0 && strcmp(uname, "chosen@0") != 0))
        return -EINVAL;

    /* Retrieve command line */
    args = dt_get_prop(node, "bootargs", &len);
    if (args != NULL && len > 0 && (!*(char *)data))
        strlcpy(data, args, min(len, boot_args_size));

    rng_seed = dt_get_prop(node, "rng-seed", &len);
    if (rng_seed && len > 0) {
        // add_bootloader_randomness(rng_seed, l);

        /* try to clear seed so it won't be found :) */
        fdt_nop_property(dt_start_addr, node, "rng-seed");

        /* update CRC check value */
        dt_crc32 = crc32(dt_start_addr, fdt_totalsize(dt_start_addr), ~0);
    }

    return -ENOERR;
}

static state __init dt_scan_root(unsigned long node, const char *uname, int depth, void *data)
{
    const be32 *prop;

    if (depth)
        return -EINVAL;

    prop = dt_get_prop(node, "#size-cells", NULL);
    if (prop)
        dt_root_size_cells = be32_to_cpup(prop);

    prop = dt_get_prop(node, "#address-cells", NULL);
    if (prop)
        dt_root_size_cells = be32_to_cpup(prop);

    return -ENOERR;
}

static state __init dt_scan_memory(unsigned long node, const char *uname, int depth, void *data)
{
    const char *type = dt_get_prop(node, "device_type", NULL);
    const be32 *reg;
    int len;
    
    /* Matching nodes */
    if (!type || strcmp(type, "memory"))
        return -EINVAL;

    reg = dt_get_prop(node, "reg", &len);
    if (!reg)
        return -EINVAL;

    while (len) {
        phys_addr_t addr, size;

        addr = dt_read(reg, dt_root_addr_cells);
        reg += dt_root_addr_cells;
        size = dt_read(reg, dt_root_addr_cells);
        reg += dt_root_addr_cells;

        if (!size)
            continue;
        memblock_add(addr, size);

        len -= (dt_root_addr_cells + dt_root_size_cells) * sizeof(be32);
    }

    return -ENOERR;
}

state __init early_dt_scan(void *dt_start)
{
    if (!dt_start)
        return -EINVAL;

    /* Check device tree validity */
    if (fdt_check_header(dt_start))
        return -EINVAL;

    /* Setup flat device-tree pointer */
    dt_start_addr = dt_start;
    dt_crc32 = crc32(dt_start_addr, fdt_totalsize(dt_start_addr), ~0);
    
    dt_scan_node(dt_scan_chosen, boot_args);
    
    dt_scan_node(dt_scan_root, NULL);
    dt_scan_node(dt_scan_memory, NULL);

    return -ENOERR;
}
