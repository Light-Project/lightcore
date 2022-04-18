/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_H_
#define _DRIVER_DT_H_

#include <state.h>
#include <slist.h>
#include <kobject.h>
#include <devtable.h>

#define DT_ADDR_CELL "#address-cells"
#define DT_SIZE_CELL "#size-cells"

#define DT_ROOT_ADDR_CELLS_DEFAULT  1
#define DT_ROOT_SIZE_CELLS_DEFAULT  1

#define DT_DEPTH_MAX  16
#define DT_PHANDLE_ARGS  16

/**
 * struct dt_attribute - describes the attributes of dt node.
 * @list: linked slist node of resources.
 * @name: name of the device tree node resource.
 * @value: data pointer of device tree node resource.
 * @len: data length of device tree node resource.
 */
struct dt_attribute {
    struct slist_head list;
    const char *name;
    const void *value;
    int len;
};

/**
 * struct dt_node - describe a device tree node.
 * @kobj: kobject object of the device tree node.
 * @path: path name of the device tree node.
 * @name: full name of the device tree node.
 * @phandle: id used for resource lookup parent node.
 * @parent: parent node of the device tree node.
 * @child: list of child nodes of the device tree node.
 * @sibling: linked list node of the child nodes of the device tree;
 * @attribute: resource list of device tree nodes.
 */
struct dt_node {
    struct kobject kobj;
    const char *path;
    const char *name;
    uint32_t phandle;

    struct dt_node *parent;
    struct slist_head child;
    struct slist_head sibling;
    struct slist_head attribute;
    struct slist_head phandle_node;
};

struct dt_phandle_args {
    const struct dt_node *node;
    int args_count;
    uint32_t args[DT_PHANDLE_ARGS];
};

struct dt_phandle_iterator {
    const struct dt_node *parent;
    const struct dt_node *node;
    const char *cells_name;
    int cell_count;

    const be32 *list_end;
    const be32 *phandle_end;

    const be32 *curr;
    uint32_t count;
    uint32_t phandle;
};

/* device tree global resources */
extern struct dt_node *dt_root;
extern struct dt_node *dt_chosen;
extern struct dt_node *dt_stdout;
extern unsigned int dt_root_addr_cells;
extern unsigned int dt_root_size_cells;
extern struct slist_head dt_phandle_list;
extern spinlock_t dt_lock;

/* device tree iteration method */
extern struct dt_node *dt_iterative_all(struct dt_node *node);
extern struct dt_node *dt_iterative_available(struct dt_node *node);
extern struct dt_node *dt_iterative_child(struct dt_node *node, struct dt_node *iter);
extern struct dt_node *dt_iterative_child_available(struct dt_node *node, struct dt_node *iter);
extern state dt_iterative_phandle_init(struct dt_phandle_iterator *iter, const struct dt_node *node, const char *listn, const char *celln, int cellc);
extern state dt_iterative_phandle_next(struct dt_phandle_iterator *iter);
extern int dt_iterative_phandle_args(struct dt_phandle_iterator *iter, uint32_t *args, int size);

#define dt_for_each_all(node) \
    for (node = dt_iterative_all(NULL); node; node = dt_iterative_all(node))

#define dt_for_each_available_all(node) \
    for (node = dt_iterative_available(NULL); node; node = dt_iterative_available(node))

#define dt_for_each_child(child, bus) \
    for (child = dt_iterative_child(bus, NULL); child; child = dt_iterative_child(bus, child))

#define dt_for_each_available_child(child, bus) \
    for (child = dt_iterative_child_available(bus, NULL); child; child = dt_iterative_child_available(bus, child))

#define dt_for_each_phandle(iter, node, listn, celln, cellc, retval) \
    for ((void)(!(retval = dt_iterative_phandle_init(iter, node, listn, celln, cellc)) && \
         !(retval = dt_iterative_phandle_next(iter))); !retval; retval = dt_iterative_phandle_next(iter))

/* device tree resource base operation method */
extern struct dt_node *dt_find_by_phandle(uint32_t phandle);
extern struct dt_attribute *dt_attribute_find(const struct dt_node *node, const char *name);
extern const void *dt_attribute_get(const struct dt_node *node, const char *name, int *len);
extern bool dt_attribute_read_bool(const struct dt_node *node, const char *name);
extern state dt_attribute_string_index(const struct dt_node *node, const char *name, const char *str);
extern state dt_attribute_read_u32_index(const struct dt_node *node, const char *name, int index, uint32_t *value);
extern state dt_attribute_read_u64_index(const struct dt_node *node, const char *name, int index, uint64_t *value);
extern state dt_attribute_read_string_index(const struct dt_node *node, const char *name, int index, const char **str);

static inline state dt_attribute_read_u32(const struct dt_node *node, const char *name, uint32_t *value)
{
    return dt_attribute_read_u32_index(node, name, 0, value);
}

static inline state dt_attribute_read_u64(const struct dt_node *node, const char *name, uint64_t *value)
{
    return dt_attribute_read_u64_index(node, name, 0, value);
}

static inline state dt_attribute_read_string(const struct dt_node *node, const char *name, const char **str)
{
    return dt_attribute_read_string_index(node, name, 0, str);
}

extern struct dt_node *dt_search_up(const struct dt_node *node, const char *name, uint32_t *value);
extern uint32_t dt_addr_cell(const struct dt_node *node);
extern uint32_t dt_size_cell(const struct dt_node *node);
extern bool dt_node_check_available(const struct dt_node *node);

/* device tree parameter parser */
extern state dt_parser_phandle_with_args_count(const struct dt_node *node, const char *list_name, const char *cells_name, int cell_count, int index, struct dt_phandle_args *args);
extern state dt_parser_phandle_with_args_map(const struct dt_node *node, const char *list_name, const char *stem_name, int index, struct dt_phandle_args *args);

static inline state dt_parser_phandle_with_args(const struct dt_node *node, const char *list_name, const char *cells_name, int index, struct dt_phandle_args *args)
{
    int cell_count = 0;

    if (cells_name)
        cell_count = -1;

    return dt_parser_phandle_with_args_count(node, list_name, cells_name, cell_count, index, args);
}

/* bus.c */
extern state dt_address(const struct dt_node *node, unsigned int index, resource_size_t *addr, resource_size_t *size);
extern unsigned int dt_address_nr(const struct dt_node *node);

/* irq.c */
extern struct dt_node *dt_irq_parent(const struct dt_node *node);
extern state dt_irq(const struct dt_node *node, unsigned int index, resource_size_t *irq);
extern unsigned int dt_irq_nr(const struct dt_node *node);

#endif
