/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"efdt: " fmt

#include <init.h>
#include <crypto.h>
#include <memory.h>
#include <mm/memblock.h>
#include <driver/dt.h>
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
        strlcpy(data, args, min(len, BOOT_PARAM_SIZE));

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

    if (depth != 0)
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
        size = dt_read(reg, dt_root_size_cells);
        reg += dt_root_size_cells;

        if (unlikely(!size))
            continue;

        memblock_add("fdt-memory", addr, size);
        len -= (dt_root_addr_cells + dt_root_size_cells) * sizeof(be32);
    }

    return -ENOERR;
}

state __init early_dt_scan(void *addr)
{
    uint32_t magic, size, version;

    if (!addr) {
        pr_warn("device tree pointer null\n");
        return -EINVAL;
    }

    if (fdt_check_header(addr)) {
        pr_err("device tree format error %p\n", addr);
        return -ENODATA;
    }

    dt_start_addr = addr;
    dt_crc32 = crc32(dt_start_addr, fdt_totalsize(dt_start_addr), ~0);

    magic   = fdt_magic(dt_start_addr);
    size    = fdt_totalsize(dt_start_addr);
    version = fdt_version(dt_start_addr);

    pr_info("device tree info:\n");
    pr_info("  magic: 0x%08x\n", magic);
    pr_info("  size: 0x%08x\n", size);
    pr_info("  version: 0x%08x\n", version);

    memblock_reserve("dts", va_to_pa(addr), size);

    dt_scan_node(dt_scan_chosen, boot_args);
    dt_scan_node(dt_scan_root, NULL);
    dt_scan_node(dt_scan_memory, NULL);

    return -ENOERR;
}
