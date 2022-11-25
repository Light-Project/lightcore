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

/**
 * dt_path_compare - match the path of a device node.
 * @node: the node to compare path.
 * @name: the path name to compare.
 */
bool dt_path_compare(const struct dt_node *node, const char *name)
{
	const char *nname;
    intptr_t length;

    nname = basename(node->path);
    length = strchrnul(nname, '@') - nname;

    return (length == strlen(name)) && !strncmp(nname, name, length);
}

/**
 * dt_find_by_phandle - find device node by phandle number.
 * @phandle: find node based by this number.
 */
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
 * dt_attribute_find - find a attribute form device node.
 * @node: device node to find attribute.
 * @name: name of the attribute.
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
 * @node: device node to read the attribute value.
 * @name: name of the attribute to be searched.
 * @len: attribute value len.
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

const void *dt_attribute_get_array(const struct dt_node *node, const char *name,
                                   int *len, size_t min, size_t max)
{
    struct dt_attribute *attribute = dt_attribute_find(node, name);

    if (!attribute)
        return ERR_PTR(-ENOENT);
    if (!attribute->value)
        return ERR_PTR(-ENODATA);
    if (attribute->len < min)
        return ERR_PTR(-EOVERFLOW);
    if (max && attribute->len > max)
        return ERR_PTR(-EOVERFLOW);

    if (len)
        *len = attribute->len;

    return attribute->value;
}
EXPORT_SYMBOL(dt_attribute_get_array);

/**
 * dt_attribute_read_bool - find and check exist from attribute.
 * @node: device node to read the attribute value.
 * @name: name of the attribute to be searched.
 * @index: index of the values.
 * @value: pointer to return value.
 */
bool dt_attribute_read_bool(const struct dt_node *node, const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, name);
    return !!attribute;
}
EXPORT_SYMBOL(dt_attribute_read_bool);

/**
 * dt_attribute_read_u32/64_index - find and read a value from attribute.
 * @node: device node to read the attribute value.
 * @name: name of the attribute to be searched.
 * @index: index of the values.
 * @value: pointer to return value.
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
 * dt_attribute_read_u8/16/32/64_index_array_range - find and read a value array from attribute.
 * @node: device node to read the attribute value.
 * @name: name of the attribute to be searched.
 * @buff: pointer to return array.
 * @index: index offset of the array.
 * @min: allowable minimum size.
 * @max: allowable maximum size.
 */
int dt_attribute_read_u8_index_array_range(const struct dt_node *node, const char *name, uint8_t *buff,
                                           unsigned int index, size_t min, size_t max)
{
    const uint8_t *value;
    int len, count;

    value = dt_attribute_get_array(
        node, name, &len,
        (sizeof(*buff) * (min + index)),
        (sizeof(*buff) * (max + index))
    );

    if (unlikely(IS_INVAL(value)))
        return PTR_INVAL(value);

    if (!max)
        len = min;
    else
        len /= sizeof(*buff);

    value += index;
    for (count = 0; count < len; ++count)
        *buff++ = *value++;

    return len;
}
EXPORT_SYMBOL(dt_attribute_read_u8_index_array_range);

int dt_attribute_read_u16_index_array_range(const struct dt_node *node, const char *name, uint16_t *buff,
                                            unsigned int index, size_t min, size_t max)
{
    const uint16_t *value;
    int len, count;

    value = dt_attribute_get_array(
        node, name, &len,
        (sizeof(*buff) * (min + index)),
        (sizeof(*buff) * (max + index))
    );

    if (unlikely(IS_INVAL(value)))
        return PTR_INVAL(value);

    if (!max)
        len = min;
    else
        len /= sizeof(*buff);

    value += index;
    for (count = 0; count < len; ++count)
        *buff++ = be16_to_cpup(value++);

    return len;
}
EXPORT_SYMBOL(dt_attribute_read_u16_index_array_range);

int dt_attribute_read_u32_index_array_range(const struct dt_node *node, const char *name, uint32_t *buff,
                                            unsigned int index, size_t min, size_t max)
{
    const uint32_t *value;
    int len, count;

    value = dt_attribute_get_array(
        node, name, &len,
        (sizeof(*buff) * (min + index)),
        (sizeof(*buff) * (max + index))
    );

    if (unlikely(IS_INVAL(value)))
        return PTR_INVAL(value);

    if (!max)
        len = min;
    else
        len /= sizeof(*buff);

    value += index;
    for (count = 0; count < len; ++count)
        *buff++ = be32_to_cpup(value++);

    return len;
}
EXPORT_SYMBOL(dt_attribute_read_u32_index_array_range);

int dt_attribute_read_u64_index_array_range(const struct dt_node *node, const char *name, uint64_t *buff,
                                            unsigned int index, size_t min, size_t max)
{
    const uint64_t *value;
    int len, count;

    value = dt_attribute_get_array(
        node, name, &len,
        (sizeof(*buff) * (min + index)),
        (sizeof(*buff) * (max + index))
    );

    if (unlikely(IS_INVAL(value)))
        return PTR_INVAL(value);

    if (!max)
        len = min;
    else
        len /= sizeof(*buff);

    value += index;
    for (count = 0; count < len; ++count)
        *buff++ = be64_to_cpup(value++);

    return len;
}
EXPORT_SYMBOL(dt_attribute_read_u64_index_array_range);

/**
 * dt_attribute_read_string_index - find and read a string from attribute.
 * @node: device node to read the attribute string.
 * @name: name of the attribute to be searched.
 * @str: pointer to return string array.
 * @index: index of the string.
 */
state dt_attribute_read_string_index_array(const struct dt_node *node, const char *name,
                                           const char **str, unsigned int index, size_t size)
{
    const char *attribute;
    int len, slen;

    attribute = dt_attribute_get(node, name, &len);
    if (!attribute)
        return -EINVAL;

    while (len && size--) {
        *str++ = attribute;
        slen = strnlen(attribute, len) + 1;
        attribute += slen;
        len -= slen;
    }

    return size ? -ENODATA : -ENOERR;
}
EXPORT_SYMBOL(dt_attribute_read_string_index_array);

/**
 * dt_attribute_string_index - find and read string index from attribute.
 * @node: device node to read the attribute string index.
 * @name: name of the attribute to be searched.
 * @str: string to read index.
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
    } while ((node = node->parent));

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
 * @node: node to check availability.
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

/**
 * dt_node_child_count - count number of child nodes.
 * @node: node to count child.
 */
unsigned int dt_node_child_count(const struct dt_node *node)
{
    const struct dt_node *child;
    unsigned int count = 0;

    dt_for_each_child(child, node)
        count++;

    return count;
}
EXPORT_SYMBOL(dt_node_child_count);
