/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <driver/dt.h>
#include <export.h>

struct dt_node *dt_root;
struct dt_node *dt_chosen;
struct dt_node *dt_stdout;

struct dt_node *dt_find_by_phandle(uint32_t phandle)
{
    struct dt_node *node;

    slist_for_each_entry(node, &dt_phandle_list, phandle_node) {
        if (node->phandle == phandle)
            return node;
    }

    return NULL;
}
EXPORT_SYMBOL(dt_find_by_phandle);

/**
 * dt_attribute_get - find a attribute form device node.
 * @node: device node to find attribute
 * @name: name of the attribute
 */
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
EXPORT_SYMBOL(dt_attribute_find);

/**
 * dt_attribute_get - find and get value from attribute.
 * @node: device node to read the attribute value
 * @name: name of the attribute to be searched
 * @len: attribute value len
 */
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
EXPORT_SYMBOL(dt_attribute_get);

/**
 * dt_attribute_read_bool - find and check exist from attribute.
 * @node: device node to read the attribute value
 * @name: name of the attribute to be searched
 * @index: index of the values
 * @value: pointer to return value
 */
bool dt_attribute_read_bool(const struct dt_node *node, const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, name);
    return !!attribute;
}
EXPORT_SYMBOL(dt_attribute_read_bool);

/**
 * dt_attribute_read_u32_index - find and read a uint32_t from attribute.
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
EXPORT_SYMBOL(dt_attribute_read_u32_index);

/**
 * dt_attribute_read_u64_index - find and read a uint64_t from attribute.
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
EXPORT_SYMBOL(dt_attribute_read_u64_index);

/**
 * dt_attribute_read_string_index - find and read a string from attribute.
 * @node: device node to read the attribute string
 * @name: name of the attribute to be searched
 * @index: index of the string
 * @str: pointer to return string
 */
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
EXPORT_SYMBOL(dt_attribute_read_string_index);

/**
 * dt_attribute_string_index - find and read string index from attribute.
 * @node: device node to read the attribute string index
 * @name: name of the attribute to be searched
 * @str: string to read index
 */
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
EXPORT_SYMBOL(dt_attribute_string_index);

/**
 * dt_search_up - search up until results are found.
 * @node: node to start searching upwards.
 * @name: name of the resource to search upwards.
 * @value: pointer to return the resource value.
 */
struct dt_node *dt_search_up(const struct dt_node *node,
                             const char *name, uint32_t *value)
{
    do {
        if (!dt_attribute_read_u32(node, name, value))
            return (struct dt_node *)node;
    } while((node = node->parent));

    return NULL;
}
EXPORT_SYMBOL(dt_search_up);

/**
 * dt_node_is_available - check if a device is available for use.
 * @node: node to check for availability.
 */
bool dt_node_is_available(const struct dt_node *node)
{
    const char *status;
    int statlen;

    status = dt_attribute_get(node, "status", &statlen);
    if (!status)
        return false;

    if (statlen > 0) {
        if (!strcmp(status, "okay") || !!strcmp(status, "ok"))
            return true;
    }

    return false;
}
EXPORT_SYMBOL(dt_node_is_available);

/**
 * dt_addr_cell - read the address cell of the node
 * @node: device node to read the address cell
 */
uint32_t dt_addr_cell(const struct dt_node *node)
{
    uint32_t cell;

    while ((node = node->parent)) {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, &cell))
            return cell;
    }

    return DT_ROOT_ADDR_CELLS_DEFAULT;
}
EXPORT_SYMBOL(dt_addr_cell);

/**
 * dt_size_cell - read the size cell of the node
 * @node: device node to read the size cell
 */
uint32_t dt_size_cell(const struct dt_node *node)
{
    uint32_t cell;

    while ((node = node->parent)) {
        if (!dt_attribute_read_u32(node, DT_ADDR_CELL, &cell))
            return cell;
    }

    return DT_ROOT_ADDR_CELLS_DEFAULT;
}
EXPORT_SYMBOL(dt_size_cell);

/**
 * dt_node_check_available - check if a device is available for use.
 * @node: node to check for availability.
 */
bool dt_node_check_available(const struct dt_node *node)
{
    const char *status;
    int statlen;

    status = dt_attribute_get(node, "status", &statlen);
    if (!status)
        return false;

    if (statlen > 0) {
        if (!strcmp(status, "okay") || !!strcmp(status, "ok"))
            return true;
    }

    return false;
}
EXPORT_SYMBOL(dt_node_check_available);

unsigned int dt_node_chile_count(const struct dt_node *node)
{
    const struct dt_node *child;
    unsigned int count = 0;

    dt_for_each_child(child, node)
        count++;

    return count;
}
EXPORT_SYMBOL(dt_node_chile_count);
