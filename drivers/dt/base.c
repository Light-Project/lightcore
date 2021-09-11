/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/dt.h>
#include <driver/dt/libfdt.h>
#include <string.h>

struct dt_node *dt_root;
struct dt_node *dt_chosen;
struct dt_node *dt_stdout;

struct dt_node *dt_for_each_all_node(struct dt_node *node)
{
    if(!node)
        return dt_root;
    else if (!slist_check_empty(&node->child))
        return slist_first_entry(&node->child, struct dt_node, sibling);
    else {
        while (slist_check_end(&node->sibling) && node->parent)
            node = node->parent;
        return slist_next_entry_or_null(node, sibling);
    }
}

struct dt_node *dt_find_by_phandle(uint32_t phandle)
{
    struct dt_node *node;

    dt_for_each_all(node)
        if (node->phandle == phandle)
            return node;

    return NULL;
}

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
state dt_attribute_read_u32_index(const struct dt_node *node, const char *name,
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
state dt_attribute_read_u64_index(const struct dt_node *node, const char *name,
                                  int index, uint64_t *value)
{
    const be64 *attribute = dt_attribute_get(node, name, NULL);

    if (!attribute)
        return -EINVAL;

    *value = be64_to_cpup(attribute + index);
    return -ENOERR;
}

state dt_attribute_read_string_index(const struct dt_node *node, const char *name,
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

state dt_attribute_string_index(const struct dt_node *node,
                                const char *name, const char *str)
{
    const char *attribute;
    int len, index;

    attribute = dt_attribute_get(node, name, &len);
    if (!attribute)
        return -EINVAL;

    len = memcount(attribute, '\0', len);

    for (index = 0; index < len; index++) {
        dt_attribute_read_string_index(node, name, index, &attribute);
        if (!strcmp(attribute, str))
            return index;
    }

    return -ENODATA;
}

struct dt_node *dt_search_up(struct dt_node *node,
                             const char *name, uint32_t *value)
{
    do {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, value))
            return node;
    } while((node = node->parent));

    return NULL;
}

/**
 * dt_addr_cell -  read the address cell of the node
 * @node: device node to read the address cell
 */
uint32_t dt_addr_cell(const struct dt_node *node)
{
    uint32_t cell;

    while((node = node->parent)) {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, &cell))
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
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, &cell))
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

    node_name = basename(node->path);
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