/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "efdt"
#define pr_fmt(fmt)	MODULE_NAME ": " fmt

#include <init.h>
#include <crypto.h>
#include <memory.h>
#include <mm/memblock.h>
#include <driver/dt.h>
#include <driver/dt/fdt.h>
#include <driver/dt/libfdt.h>
#include <printk.h>

uint32_t dt_crc32;
void *dt_start_addr;

/**
 * dt_scan_node - scan flattened tree blob and call callback on each.
 * @iter: callback function.
 * @data: context data pointer.
 */
state __init dt_scan_node(state (*iter)(unsigned long node, const char *uname,
                          int depth, void *data), void *data)
{
    const void *node = dt_start_addr;
    const char *uname;
    int offset, depth = -1;
    state ret = -ENODATA;

    if (!node)
        return -EINVAL;

    for (offset = fdt_next_node(node, -1, &depth);
         offset >= 0 && depth >= 0 && ret;
         offset = fdt_next_node(node, offset, &depth)) {
        uname = fdt_get_name(node, offset, NULL);
        ret = iter(offset, uname, depth, data);
    }

    return ret;
}

/**
 * dt_scan_subnode - scan sub-nodes call callback on each.
 * @iter: callback function.
 * @data: context data pointer.
 */
state __init dt_scan_subnode(unsigned long parent, int (*iter)(unsigned long node,
                             const char *uname,void *data), void *data)
{
    const void *blob = dt_start_addr;
    const char *pathp;
    int node;
    state ret = -ENODATA;

    fdt_for_each_subnode(node, blob, parent) {
        pathp = fdt_get_name(blob, node, NULL);
        ret = iter(node, pathp, data);
        if (ret)
            return ret;
    }

    return -ENOERR;
}

int __init dt_get_subnode_by_name(unsigned long node, const char *uname)
{
    return fdt_subnode_offset(dt_start_addr, node, uname);
}

const __init void *dt_get_prop(unsigned long node, const char *name, int *size)
{
    return fdt_getprop(dt_start_addr, node, name, size);
}

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

    dt_scan_node(dt_scan_chosen, boot_args);
    dt_scan_node(dt_scan_root, NULL);
    dt_scan_node(dt_scan_memory, NULL);

    memblock_reserve("dts", va_to_pa(addr), size);

    return -ENOERR;
}
