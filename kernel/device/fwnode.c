/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <fwnode.h>
#include <mhelper.h>
#include <export.h>

#define GENERIC_FWNODE_NAME(name, operation, rtype, errval, args, ...)  \
rtype fwnode_##name(const struct fwnode *node, ##__VA_ARGS__)           \
{                                                                       \
    struct fwnode_ops *ops;                                             \
                                                                        \
    if (unlikely(!node))                                                \
        return errval;                                                  \
                                                                        \
    ops = node->ops;                                                    \
    if (unlikely(!ops || !ops->operation))                              \
        return errval;                                                  \
                                                                        \
    return ops->operation args;                                         \
}                                                                       \
EXPORT_SYMBOL(fwnode_##name)

#define GENERIC_FWNODE_OP(operation, rtype, errval, args, ...) \
    GENERIC_FWNODE_NAME(operation, operation, rtype, errval, args, ##__VA_ARGS__)

GENERIC_FWNODE_OP(node_name, const char *, NULL, (node));
GENERIC_FWNODE_OP(node_path, const char *, NULL, (node));
GENERIC_FWNODE_OP(node_available, bool, true, (node));
GENERIC_FWNODE_OP(node_container, void *, NULL, (node));
GENERIC_FWNODE_OP(node_parent, struct fwnode *, NULL, (node));
GENERIC_FWNODE_OP(next_child, struct fwnode *, NULL, (node, iter), struct fwnode *iter);
GENERIC_FWNODE_OP(find_child, struct fwnode *, NULL, (node, name), const char *name);
GENERIC_FWNODE_OP(attribute_present, bool, false, (node, name), const char *name);
GENERIC_FWNODE_OP(attribute_read_value_index_array, state, -EOPNOTSUPP, (node, name, esize, val, index, len), const char *name, unsigned int esize, void *val, unsigned int index, size_t len);
GENERIC_FWNODE_OP(attribute_read_string_index_array, state, -EOPNOTSUPP, (node, name, val, index, len), const char *name, const char **val, unsigned int index, size_t len);
GENERIC_FWNODE_NAME(attribute_read_string, attribute_read_string_index_array, state, -EOPNOTSUPP, (node, name, val, 0, 1), const char *name, const char **val);
GENERIC_FWNODE_NAME(attribute_read_string_index, attribute_read_string_index_array, state, -EOPNOTSUPP, (node, name, val, index, 1), const char *name, const char **val, unsigned int index);
GENERIC_FWNODE_NAME(attribute_read_string_array, attribute_read_string_index_array, state, -EOPNOTSUPP, (node, name, val, 0, len), const char *name, const char **val, size_t len);

struct fwnode *fwnode_next_available_child(const struct fwnode *node, struct fwnode *iter)
{
    struct fwnode *child = NULL;

	do {
		child = fwnode_next_child(node, child);
		if (!child)
			return NULL;
	} while (!fwnode_node_available(child));

    return child;
}
EXPORT_SYMBOL(fwnode_next_available_child);

#define GENERIC_FWNODE_CHILD_COUNT(operation)                           \
unsigned int fwnode_##operation##_count(const struct fwnode *node)      \
{                                                                       \
    unsigned int count = 0;                                             \
    struct fwnode *iter;                                                \
                                                                        \
    fwnode_for_each_##operation(iter, node)                             \
        count++;                                                        \
                                                                        \
    return count;                                                       \
}                                                                       \
EXPORT_SYMBOL(fwnode_##operation##_count)

GENERIC_FWNODE_CHILD_COUNT(child);
GENERIC_FWNODE_CHILD_COUNT(available_child);

#define GENERIC_FWNODE_VALUE_OP(func, type)                                                                             \
state fwnode_attribute_read_##func(const struct fwnode *node, const char *name, type *val)                              \
{                                                                                                                       \
    return fwnode_attribute_read_value_index_array(node, name, sizeof(type), val, 0, 1);                                \
}                                                                                                                       \
EXPORT_SYMBOL(fwnode_attribute_read_##func);                                                                            \
                                                                                                                        \
state fwnode_attribute_read_##func##_count(const struct fwnode *node, const char *name)                                 \
{                                                                                                                       \
    return fwnode_attribute_read_value_index_array(node, name, sizeof(type), NULL, 0, 0);                               \
}                                                                                                                       \
EXPORT_SYMBOL(fwnode_attribute_read_##func##_count);                                                                    \
                                                                                                                        \
state fwnode_attribute_read_##func##_index(const struct fwnode *node, const char *name, type *val, unsigned int index)  \
{                                                                                                                       \
    return fwnode_attribute_read_value_index_array(node, name, sizeof(type), val, index, 1);                            \
}                                                                                                                       \
EXPORT_SYMBOL(fwnode_attribute_read_##func##_index);                                                                    \
                                                                                                                        \
state fwnode_attribute_read_##func##_array(const struct fwnode *node, const char *name, type *val, size_t len)          \
{                                                                                                                       \
    return fwnode_attribute_read_value_index_array(node, name, sizeof(type), val, 0, len);                              \
}                                                                                                                       \
EXPORT_SYMBOL(fwnode_attribute_read_##func##_array)

GENERIC_FWNODE_VALUE_OP(u8, uint8_t);
GENERIC_FWNODE_VALUE_OP(u16, uint16_t);
GENERIC_FWNODE_VALUE_OP(u32, uint32_t);
GENERIC_FWNODE_VALUE_OP(u64, uint64_t);

#define GENERIC_DEVICE_OP(operation, rtype, errval, args, ...)          \
rtype device_##operation(const struct device *device, ##__VA_ARGS__)    \
{                                                                       \
    const struct fwnode *node = device->fwnode;                         \
                                                                        \
    if (!node)                                                          \
        return errval;                                                  \
                                                                        \
    return fwnode_##operation args;                                     \
}                                                                       \
EXPORT_SYMBOL(device_##operation)

GENERIC_DEVICE_OP(node_name, const char *, NULL, (node));
GENERIC_DEVICE_OP(node_path, const char *, NULL, (node));
GENERIC_DEVICE_OP(node_available, bool, true, (node));
GENERIC_DEVICE_OP(node_container, void *, NULL, (node));
GENERIC_DEVICE_OP(node_parent, struct fwnode *, NULL, (node));
GENERIC_DEVICE_OP(next_child, struct fwnode *, NULL, (node, iter), struct fwnode *iter);
GENERIC_DEVICE_OP(find_child, struct fwnode *, NULL, (node, name), const char *name);
GENERIC_DEVICE_OP(attribute_present, bool, false, (node, name), const char *name);
GENERIC_DEVICE_OP(attribute_read_value_index_array, state, -EOPNOTSUPP, (node, name, esize, val, index, len), const char *name, unsigned int esize, void *val, unsigned int index, size_t len);
GENERIC_DEVICE_OP(attribute_read_string_index_array, state, -EOPNOTSUPP, (node, name, val, index, len), const char *name, const char **val, unsigned int index, size_t len);
GENERIC_DEVICE_OP(attribute_read_string, state, -EOPNOTSUPP, (node, name, val), const char *name, const char **val);
GENERIC_DEVICE_OP(attribute_read_string_index, state, -EOPNOTSUPP, (node, name, val, index), const char *name, const char **val, unsigned int index);
GENERIC_DEVICE_OP(attribute_read_string_array, state, -EOPNOTSUPP, (node, name, val, len), const char *name, const char **val, size_t len);
GENERIC_DEVICE_OP(child_count, unsigned int, 0, (node));
GENERIC_DEVICE_OP(available_child_count, unsigned int, 0, (node));

#define GENERIC_DEVICE_VALUE_OP(func, type)                                                                                 \
state device_attribute_read_##func(const struct device *device, const char *name, type *val)                                \
{                                                                                                                           \
    return device_attribute_read_value_index_array(device, name, sizeof(type), val, 0, 1);                                  \
}                                                                                                                           \
EXPORT_SYMBOL(device_attribute_read_##func);                                                                                \
                                                                                                                            \
state device_attribute_read_##func##_count(const struct device *device, const char *name)                                   \
{                                                                                                                           \
    return device_attribute_read_value_index_array(device, name, sizeof(type), NULL, 0, 0);                                 \
}                                                                                                                           \
EXPORT_SYMBOL(device_attribute_read_##func##_count);                                                                        \
                                                                                                                            \
state device_attribute_read_##func##_index(const struct device *device, const char *name, type *val, unsigned int index)    \
{                                                                                                                           \
    return device_attribute_read_value_index_array(device, name, sizeof(type), val, index, 1);                              \
}                                                                                                                           \
EXPORT_SYMBOL(device_attribute_read_##func##_index);                                                                        \
                                                                                                                            \
state device_attribute_read_##func##_array(const struct device *device, const char *name, type *val, size_t len)            \
{                                                                                                                           \
    return device_attribute_read_value_index_array(device, name, sizeof(type), val, 0, len);                                \
}                                                                                                                           \
EXPORT_SYMBOL(device_attribute_read_##func##_array)

GENERIC_DEVICE_VALUE_OP(u8, uint8_t);
GENERIC_DEVICE_VALUE_OP(u16, uint16_t);
GENERIC_DEVICE_VALUE_OP(u32, uint32_t);
GENERIC_DEVICE_VALUE_OP(u64, uint64_t);
