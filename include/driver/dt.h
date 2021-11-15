/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_H_
#define _DRIVER_DT_H_

#include <state.h>
#include <slist.h>
#include <kobject.h>
#include <devtable.h>

extern struct dt_node *dt_root;
extern struct dt_node *dt_chosen;
extern struct dt_node *dt_stdout;

extern unsigned int dt_root_addr_cells;
extern unsigned int dt_root_size_cells;

#define DT_ADDR_CELL    "#address-cells"
#define DT_SIZE_CELL    "#size-cells"

#define DT_ROOT_ADDR_CELLS_DEFAULT  1
#define DT_ROOT_SIZE_CELLS_DEFAULT  1

struct dt_attribute {
    const char *name;
    const void *value;
    int len;
    slist_t list;
};

struct dt_node {
    struct kobject kobj;
    const char *name;
    const char *path;
    uint32_t phandle;

    struct dt_node      *parent;
    struct slist_head   child;
    struct slist_head   sibling;
    struct slist_head   attribute;
};

extern struct dt_node *dt_root;
extern struct dt_node *dt_chosen;
extern struct dt_node *dt_stdout;

struct dt_node *dt_find_by_phandle(uint32_t phandle);
struct dt_node *dt_for_each_all_node(struct dt_node *prev);
struct dt_attribute *dt_attribute_find(const struct dt_node *node, const char *name);
const void *dt_attribute_get(const struct dt_node *node, const char *name, int *len);

#define dt_for_each_all(node) \
    for (node = dt_for_each_all_node(NULL); node; node = dt_for_each_all_node(node))
#define dt_have_child(node) \
    slist_first_entry_or_null(&prev->child, struct dt_node, sibling)
#define dt_for_each_child(_child, bus) \
    slist_for_each_entry(_child, &bus->child, sibling)

bool  dt_attribute_read_bool(const struct dt_node *node, const char *name);
state dt_attribute_string_index(const struct dt_node *node, const char *name, const char *str);
state dt_attribute_read_u32_index(const struct dt_node *node, const char *name, int index, uint32_t *value);
state dt_attribute_read_u64_index(const struct dt_node *node, const char *name, int index, uint64_t *value);
state dt_attribute_read_string_index(const struct dt_node *node, const char *name, int index, const char **str);

#define dt_attribute_read_u32(node, name, val) \
    dt_attribute_read_u32_index(node, name, 0, val)
#define dt_attribute_read_u64(node, name, val) \
    dt_attribute_read_u64_index(node, name, 0, val)
#define dt_attribute_read_struct(node, name, val) \
    dt_attribute_read_string_index(node, name, 0, val)

struct dt_node *dt_search_up(const struct dt_node *node, const char *name, uint32_t *value);
uint32_t dt_addr_cell(const struct dt_node *node);
uint32_t dt_size_cell(const struct dt_node *node);

bool dt_attribute_match(const struct dt_node *node, const char *atname, const char *val);
bool dt_match_name(const struct dt_node *node, const char *name);
bool dt_match(const struct dt_device_id *id, const struct dt_node *node);

/* bus.c */
state dt_address(const struct dt_node *node, unsigned int index, resource_size_t *addr, resource_size_t *size);
unsigned int dt_address_nr(const struct dt_node *node);

/* irq.c */
struct dt_node *dt_irq_parent(const struct dt_node *node);
state dt_irq(const struct dt_node *node, unsigned int index, resource_size_t *irq);
unsigned int dt_irq_nr(const struct dt_node *node);

#endif
