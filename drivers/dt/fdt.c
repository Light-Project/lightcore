/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"fdt: " fmt

#include <kmalloc.h>
#include <driver/dt.h>
#include <driver/dt/fdt.h>
#include <driver/dt/libfdt.h>
#include <printk.h>

#define DT_DEPTH_MAX    16

uint32_t dt_crc32;
void *dt_start_addr;
unsigned int dt_root_addr_cells = DT_ROOT_ADDR_CELLS_DEFAULT;
unsigned int dt_root_size_cells = DT_ROOT_SIZE_CELLS_DEFAULT;

state __init dt_scan_node(state (*fn)(unsigned long node, const char *uname,
                                      int depth, void *data), void *data)
{
    const void *node = dt_start_addr;
    const char *uname;
    int offset, rc = -1, depth = -1;

    if (!node)
        return -EINVAL;

    for (offset = fdt_next_node(node, -1, &depth);
         offset >= 0 && depth >= 0 && rc;
         offset = fdt_next_node(node, offset, &depth)) {
        uname = fdt_get_name(node, offset, NULL);
        rc = fn(offset, uname, depth, data);
    }

    return rc;
}

/**
 * dt_scan_subnode - scan sub-nodes call callback on each.
 * @it: callback function
 * @data: context data pointer
 */
int __init dt_scan_subnode(unsigned long parent, int (*it)(unsigned long node,
                           const char *uname,void *data), void *data)
{
    const void *blob = dt_start_addr;
    int node;

    fdt_for_each_subnode(node, blob, parent) {
        const char *pathp;
        int rc;

        pathp = fdt_get_name(blob, node, NULL);
        rc = it(node, pathp, data);
        if (rc)
            return rc;
    }

    return 0;
}

int dt_get_subnode_by_name(unsigned long node, const char *uname)
{
    return fdt_subnode_offset(dt_start_addr, node, uname);
}

const void *dt_get_prop(unsigned long node, const char *name, int *size)
{
    return fdt_getprop(dt_start_addr, node, name, size);
}

static state populate_attribute(const void *blob, int offset, struct dt_node *node)
{
    struct dt_attribute *att;
    int cur, len;

    for (cur = fdt_first_property_offset(blob, offset);
         cur >= 0;
         cur = fdt_next_property_offset(blob, cur)) {
        const be32 *value;
        const char *name;

        value = fdt_getprop_by_offset(blob, cur, &name, &len);
        if (!value) {
            pr_warn("failed to locate property at 0x%x\n", cur);
            continue;
        }

        if (!name) {
            pr_warn("failed to find locate name at 0x%x\n", cur);
            continue;
        }

        att = kzalloc(sizeof(*att), GFP_KERNEL);
        if (!att)
            goto error;

        if (!strcmp(name, "phandle"))
            node->phandle = be32_to_cpup(value);

        att->name   = name;
        att->len    = len;
        att->value  = value;
        slist_add(&node->attribute, &att->list);
    }

    return -ENOERR;

error:
    slist_for_each_entry(att, &node->attribute, list)
        kfree(att);

    return -ENOMEM;
}

static inline state __init populate_node(const void *blob, int offset,
                                         struct dt_node *parent, struct dt_node **nodep)
{
    struct dt_node *node;
	const char *path;
    char *fn;
    state ret;
    int len;

    path = fdt_get_name(blob, offset, &len);
    if (!path) {
        return -ENOERR;
    }

    *nodep = node = kzalloc(sizeof(*node) + (++len), GFP_KERNEL);
    if (!node)
        return -ENOMEM;

    node->path = fn = (char *)node + sizeof(*node);
    memcpy(fn, path, len);

    if (parent) {
        node->parent = parent;
        slist_add(&parent->child, &node->sibling);
    }

    ret = populate_attribute(blob, offset, node);
    if (ret) {
        kfree(node);
        return ret;
    }

    node->name = dt_attribute_get(node, "name", NULL);
    if (!node->name)
        node->name = "none";

    return -ENOERR;
}

static inline struct dt_node * __init populate_bus(const void *blob, struct dt_node *parent)
{
    int offset, depth = 0, sdepth = 0;
    struct dt_node *nodes[DT_DEPTH_MAX], *bus = NULL;

    if (parent)
        depth = sdepth = 1;

    nodes[depth] = parent;

    for (offset = 0;
         offset >= 0 && depth >= sdepth;
         offset = fdt_next_node(blob, offset, &depth)) {

        if (unlikely(depth >= DT_DEPTH_MAX))
            continue;

        if (populate_node(blob, offset,
            nodes[depth], &nodes[depth + 1]))
            return NULL;

        if (!bus)
            bus = nodes[depth + 1];
    }

    return bus;
}

void __init dt_init(void)
{
    dt_root = populate_bus(dt_start_addr, NULL);
}
