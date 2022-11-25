/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FWNODE_H_
#define _FWNODE_H_

#include <device.h>
#include <bitflags.h>

enum fwnode_flags {
    __FWNODE_IS_DTREE   = 0,
    __FWNODE_IS_ACPI    = 1,
};

#define FWNODE_IS_DTREE BIT(__FWNODE_IS_DTREE)
#define FWNODE_IS_ACPI  BIT(__FWNODE_IS_ACPI)

/**
 * struct fwnode - describe the a firmware node.
 * @dev: points to the parent device of the firmware node.
 * @ops: operations method of firmware node.
 */
struct fwnode {
    struct device *dev;
    struct fwnode_ops *ops;
    unsigned long flags;
};

GENERIC_STRUCT_BITOPS(fwnode, struct fwnode, flags, true)
GENERIC_STRUCT_FLAG(fwnode, struct fwnode, flags, dtree, __FWNODE_IS_DTREE)
GENERIC_STRUCT_FLAG(fwnode, struct fwnode, flags, acpi, __FWNODE_IS_DTREE)

/**
 * struct fwnode_ops - describe the operations of firmware node.
 * @node_name: get the name of the firmware node.
 * @node_path: get the path of the firmware node.
 * @node_available: check whether the firmware node is available.
 * @next_child: get the next child node in an iteration.
 * @find_child: find the corresponding child node by name.
 * @attribute_present: check whether the attribute is available.
 * @attribute_read_value_index_array: read an array of integer properties.
 * @attribute_read_string_index_array: read an array of string properties.
 */
struct fwnode_ops {
    const char *(*node_name)(const struct fwnode *node);
    const char *(*node_path)(const struct fwnode *node);
    bool (*node_available)(const struct fwnode *node);
    void *(*node_container)(const struct fwnode *node);
    struct fwnode *(*node_parent)(const struct fwnode *node);
    struct fwnode *(*next_child)(const struct fwnode *node, struct fwnode *iter);
    struct fwnode *(*find_child)(const struct fwnode *node, const char *name);
    bool (*attribute_present)(const struct fwnode *fwnode, const char *name);
    state (*attribute_read_value_index_array)(const struct fwnode *fwnode, const char *name, unsigned int esize, void *val, unsigned int index, size_t len);
    state (*attribute_read_string_index_array)(const struct fwnode *fwnode, const char *name, const char **val, unsigned int index, size_t len);
};

extern const char *fwnode_node_name(const struct fwnode *node);
extern const char *fwnode_node_path(const struct fwnode *node);
extern bool fwnode_node_available(const struct fwnode *node);
extern void *fwnode_node_container(const struct fwnode *node);
extern struct fwnode *fwnode_node_parent(const struct fwnode *node);
extern struct fwnode *fwnode_next_child(const struct fwnode *node, struct fwnode *iter);
extern struct fwnode *fwnode_find_child(const struct fwnode *node, const char *name);
extern bool fwnode_attribute_present(const struct fwnode *node, const char *name);
extern state fwnode_attribute_read_value_index_array(const struct fwnode *node, const char *name, unsigned int esize, void *val, unsigned int index, size_t len);
extern state fwnode_attribute_read_string_index_array(const struct fwnode *node, const char *name, const char **val, unsigned int index, size_t len);
extern state fwnode_attribute_read_string(const struct fwnode *node, const char *name, const char **val);
extern state fwnode_attribute_read_string_index(const struct fwnode *node, const char *name, const char **val, unsigned int index);
extern state fwnode_attribute_read_string_array(const struct fwnode *node, const char *name, const char **val, size_t len);

extern struct fwnode *fwnode_next_available_child(const struct fwnode *node, struct fwnode *iter);
extern unsigned int fwnode_child_count(const struct fwnode *node);
extern unsigned int fwnode_available_child_count(const struct fwnode *node);

extern state fwnode_attribute_read_u8(const struct fwnode *node, const char *name, uint8_t *val);
extern state fwnode_attribute_read_u16(const struct fwnode *node, const char *name, uint16_t *val);
extern state fwnode_attribute_read_u32(const struct fwnode *node, const char *name, uint32_t *val);
extern state fwnode_attribute_read_u64(const struct fwnode *node, const char *name, uint64_t *val);
extern state fwnode_attribute_read_u8_count(const struct fwnode *node, const char *name);
extern state fwnode_attribute_read_u16_count(const struct fwnode *node, const char *name);
extern state fwnode_attribute_read_u32_count(const struct fwnode *node, const char *name);
extern state fwnode_attribute_read_u64_count(const struct fwnode *node, const char *name);
extern state fwnode_attribute_read_u8_index(const struct fwnode *node, const char *name, uint8_t *val, unsigned int index);
extern state fwnode_attribute_read_u16_index(const struct fwnode *node, const char *name, uint16_t *val, unsigned int index);
extern state fwnode_attribute_read_u32_index(const struct fwnode *node, const char *name, uint32_t *val, unsigned int index);
extern state fwnode_attribute_read_u64_index(const struct fwnode *node, const char *name, uint64_t *val, unsigned int index);
extern state fwnode_attribute_read_u8_array(const struct fwnode *node, const char *name, uint8_t *val, size_t len);
extern state fwnode_attribute_read_u16_array(const struct fwnode *node, const char *name, uint16_t *val, size_t len);
extern state fwnode_attribute_read_u32_array(const struct fwnode *node, const char *name, uint32_t *val, size_t len);
extern state fwnode_attribute_read_u64_array(const struct fwnode *node, const char *name, uint64_t *val, size_t len);

extern const char *device_node_name(const struct device *device);
extern const char *device_node_path(const struct device *device);
extern bool device_node_available(const struct device *device);
extern void *device_node_container(const struct device *device);
extern struct fwnode *device_node_parent(const struct device *device);
extern struct fwnode *device_next_child(const struct device *device, struct fwnode *iter);
extern struct fwnode *device_next_available_child(const struct device *device, struct fwnode *iter);
extern struct fwnode *device_find_child(const struct device *device, const char *name);
extern bool device_attribute_present(const struct device *device, const char *name);
extern state device_attribute_read_value_index_array(const struct device *device, const char *name, unsigned int esize, void *val, unsigned int index, size_t len);
extern state device_attribute_read_string_index_array(const struct device *device, const char *name, const char **val, unsigned int index, size_t len);
extern state device_attribute_read_string(const struct device *device, const char *name, const char **val);
extern state device_attribute_read_string_index(const struct device *device, const char *name, const char **val, unsigned int index);
extern state device_attribute_read_string_array(const struct device *device, const char *name, const char **val, size_t len);
extern unsigned int device_child_count(const struct device *device);
extern unsigned int device_available_child_count(const struct device *device);

extern state device_attribute_read_u8(const struct device *device, const char *name, uint8_t *val);
extern state device_attribute_read_u16(const struct device *device, const char *name, uint16_t *val);
extern state device_attribute_read_u32(const struct device *device, const char *name, uint32_t *val);
extern state device_attribute_read_u64(const struct device *device, const char *name, uint64_t *val);
extern state device_attribute_read_u8_count(const struct device *device, const char *name);
extern state device_attribute_read_u16_count(const struct device *device, const char *name);
extern state device_attribute_read_u32_count(const struct device *device, const char *name);
extern state device_attribute_read_u64_count(const struct device *device, const char *name);
extern state device_attribute_read_u8_index(const struct device *device, const char *name, uint8_t *val, unsigned int index);
extern state device_attribute_read_u16_index(const struct device *device, const char *name, uint16_t *val, unsigned int index);
extern state device_attribute_read_u32_index(const struct device *device, const char *name, uint32_t *val, unsigned int index);
extern state device_attribute_read_u64_index(const struct device *device, const char *name, uint64_t *val, unsigned int index);
extern state device_attribute_read_u8_array(const struct device *device, const char *name, uint8_t *val, size_t len);
extern state device_attribute_read_u16_array(const struct device *device, const char *name, uint16_t *val, size_t len);
extern state device_attribute_read_u32_array(const struct device *device, const char *name, uint32_t *val, size_t len);
extern state device_attribute_read_u64_array(const struct device *device, const char *name, uint64_t *val, size_t len);

/**
 * fwnode_for_each_child - iterate over all children of this fwnode.
 * @iter: iterator context of firmware node.
 * @fwnode: firmware node to iterate over.
 */
#define fwnode_for_each_child(iter, fwnode)             \
    for (iter = fwnode_next_child(fwnode, NULL); iter;  \
         iter = fwnode_next_child(fwnode, iter))

/**
 * device_for_each_child - iterate over all children of this device.
 * @iter: iterator context of firmware node.
 * @device: device node to iterate over.
 */
#define device_for_each_child(iter, device)             \
    for (iter = device_next_child(device, NULL); iter;  \
         iter = device_next_child(device, iter))

/**
 * fwnode_for_each_available_child - iterate over all available children of this fwnode.
 * @iter: iterator context of firmware node.
 * @fwnode: firmware node to iterate over.
 */
#define fwnode_for_each_available_child(iter, fwnode)               \
    for (iter = fwnode_next_available_child(fwnode, NULL); iter;    \
         iter = fwnode_next_available_child(fwnode, iter))

/**
 * device_for_each_available_child - iterate over all available children of this device.
 * @iter: iterator context of firmware node.
 * @device: device node to iterate over.
 */
#define device_for_each_available_child(iter, device)               \
    for (iter = device_next_available_child(device, NULL); iter;    \
         iter = device_next_available_child(device, iter))

#endif  /* _FWNODE_H_ */
