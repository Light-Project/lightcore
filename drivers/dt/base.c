/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Equipment tree basic operation
 *
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 * 
 */

#include <driver/dt.h>
#include <driver/dt/libfdt.h>
#include <string.h>

struct dt_node *dt_root;
struct dt_node *dt_chosen;
struct dt_node *dt_stdout;

struct dt_attribute *dt_attribute_find(const struct dt_node *node, 
                                       const char *name)
{
    struct dt_attribute *attribute;
    slist_for_each_entry(attribute, &node->attribute, list) {
        if (!strcmp(attribute->name, name))
            return attribute;
    }
    return NULL;
}

const void *dt_attribute_get(const struct dt_node *node, 
                             const char *name, int *len)
{
    struct dt_attribute *attribute;

    attribute = dt_attribute_find(node, name);
    if (!attribute)
        return NULL;
    if (len)
        *len = attribute->len;
    return attribute->value;
}

bool dt_attribute_read_bool(const struct dt_node *node, const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, name);
    return !!attribute;
}

/**
 * dt_attribute_read_u32 - find and read a uint32_t from attribute.
 * @node: device node to read the attribute value
 * @name: name of the attribute to be searched
 * @index: index of the values
 * @value: pointer to return value
 */
state dt_attribute_read_u32(const struct dt_node *node, const char *name,
                            int index, uint32_t *value)
{
    const be32 *attribute = dt_attribute_get(node, name, NULL);

    if (!attribute)
        return -EINVAL;

    *value = be32_to_cpup(attribute + index);
    return -ENOERR;
}

/**
 * dt_attribute_read_u64 - find and read a uint64_t from attribute.
 * @node: device node to read the attribute value
 * @name: name of the attribute to be searched
 * @index: index of the values
 * @value: pointer to return value
 */
state dt_attribute_read_u64(const struct dt_node *node, const char *name,
                            int index, uint64_t *value)
{
    const be64 *attribute = dt_attribute_get(node, name, NULL);

    if (!attribute)
        return -EINVAL;

    *value = be64_to_cpup(attribute + index);
    return -ENOERR;
}

state dt_attribute_read_string(const struct dt_node *node, const char *name, 
                               int index, const char **str)
{
    const char *attribute = dt_attribute_get(node, name, NULL);
    unsigned int len;

    if (!attribute)
        return -EINVAL;

    while (index--) {
        len = strlen(attribute);
        attribute += len;
    }
    
    *str = attribute;
    return -ENOERR;
}

struct dt_node *dt_for_each_all(struct dt_node *prev)
{
    struct dt_node *node;
    if(!prev)
        node = dt_root;
    else if (!slist_check_empty(&prev->child))
        node = slist_first_entry(&prev->child, struct dt_node, sibling);
    else {
        node = prev;
        while (node->parent && slist_check_empty(&prev->sibling))
            node = node->parent;
        node = slist_next_entry(node, sibling);
    }
    return node;
}

/**
 * dt_addr_cell -  read the address cell of the node
 * @node: device node to read the address cell
 */
uint32_t dt_addr_cell(const struct dt_node *node)
{
    uint32_t cell;

    while((node = node->parent)) {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, 0, &cell))
            return cell;
    }
    
    return DT_ROOT_ADDR_CELLS_DEFAULT;
}

/**
 * dt_size_cell -  read the size cell of the node
 * @node: device node to read the size cell
 */
uint32_t dt_size_cell(const struct dt_node *node)
{
    uint32_t cell;

    while((node = node->parent)) {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, 0, &cell))
            return cell;
    }
    
    return DT_ROOT_ADDR_CELLS_DEFAULT;
}

/* Function for matching */

bool dt_attribute_match(const struct dt_node *node, 
                        const char *atname, const char *val)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, atname);
    return attribute && !strcmp(attribute->value, val);
}

bool dt_match_name(const struct dt_node *node, const char *name)
{   
    const char *node_name;
    int len;

    node_name = kbasename(node->path);
	len = strchrnul(name, '@') - node_name;
	return (strlen(node_name) == len) && 
           (!strncmp(node_name, name, len));
}

bool dt_match(const struct dt_device_id *id, const struct dt_node *node)
{
    if (dt_attribute_match(node, "compatible", id->compatible))
        return true;
    if (dt_attribute_match(node, "device_type", id->type))
        return true;
    if (dt_match_name(node, id->name))
        return true;
    return false;
}