/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <fwnode.h>
#include <driver/dt.h>

static const char *dt_fwnode_node_name(const struct fwnode *fwnode)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return node->name;
}

static const char *dt_fwnode_node_path(const struct fwnode *fwnode)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return node->path;
}

static bool dt_fwnode_node_available(const struct fwnode *fwnode)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return dt_node_check_available(node);
}

static struct fwnode *dt_fwnode_node_parent(const struct fwnode *fwnode)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return &node->parent->fwnode;
}

static struct fwnode *dt_fwnode_next_child(const struct fwnode *fwnode, struct fwnode *iter)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    struct dt_node *diter = fwnode_to_dt_safe(iter);
    diter = dt_iterative_child(node, diter);
    return diter ? (struct fwnode *)&diter->fwnode : NULL;
}

static struct fwnode *dt_fwnode_find_child(const struct fwnode *fwnode, const char *name)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    struct dt_node *iter;

    dt_for_each_child(iter, node) {
        if (dt_path_compare(iter, name))
            return &iter->fwnode;
    }

    return NULL;
}

static bool dt_fwnode_attribute_present(const struct fwnode *fwnode, const char *name)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return dt_attribute_read_bool(node, name);
}

static state dt_fwnode_attribute_read_value_array(const struct fwnode *fwnode, const char *name, unsigned int esize,
                                                  void *val, unsigned int index, size_t len)
{
    struct dt_node *node = fwnode_to_dt(fwnode);

    switch (esize) {
        case 1: default:
            return dt_attribute_read_u8_index_array(node, name, val, index, len);
        case 2:
            return dt_attribute_read_u16_index_array(node, name, val, index, len);
        case 4:
            return dt_attribute_read_u32_index_array(node, name, val, index, len);
        case 8:
            return dt_attribute_read_u64_index_array(node, name, val, index, len);
    }
}

static state dt_fwnode_attribute_read_string_array(const struct fwnode *fwnode, const char *name,
                                                   const char **val, unsigned int index, size_t len)
{
    struct dt_node *node = fwnode_to_dt(fwnode);
    return dt_attribute_read_string_index_array(node, name, val, index, len);
}

struct fwnode_ops dt_fwnode_ops = {
    .node_name = dt_fwnode_node_name,
    .node_path = dt_fwnode_node_path,
    .node_available = dt_fwnode_node_available,
    .node_parent = dt_fwnode_node_parent,
    .next_child = dt_fwnode_next_child,
    .find_child = dt_fwnode_find_child,
    .attribute_present = dt_fwnode_attribute_present,
    .attribute_read_value_index_array = dt_fwnode_attribute_read_value_array,
    .attribute_read_string_index_array = dt_fwnode_attribute_read_string_array,
};
