/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_H_
#define _DRIVER_DT_H_

#include <state.h>
#include <slist.h>
#include <kernel/kobject.h>
#include <mod_devicetable.h>

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
    const char *path;
    struct kobject kobj;

    struct dt_node      *parent;
    struct slist_head   child;
    struct slist_head   sibling;
    struct slist_head   attribute;

    void *driver_data;
};

extern struct dt_node *dt_root;
extern struct dt_node *dt_chosen;
extern struct dt_node *dt_stdout;

#define dt_have_child(node) \
    slist_first_entry_or_null(&prev->child, struct dt_node, sibling)
#define dt_for_each_child(_child, bus) \
    slist_for_each_entry(_child, &bus->child, sibling)

/* base.c */
struct dt_node *dt_for_each_all(struct dt_node *prev);
struct dt_attribute *dt_attribute_find(const struct dt_node *node, const char *name);
const void *dt_attribute_get(const struct dt_node *node, const char *name, int *len);

bool dt_attribute_read_bool(const struct dt_node *node, const char *name);
state dt_attribute_read_u32(const struct dt_node *node, const char *name, int index, uint32_t *value);
state dt_attribute_read_u64(const struct dt_node *node, const char *name, int index, uint64_t *value);
state dt_attribute_read_string(const struct dt_node *node, const char *name, int index, const char **str);

uint32_t dt_addr_cell(const struct dt_node *node);
uint32_t dt_size_cell(const struct dt_node *node);

bool dt_match(const struct dt_device_id *id, const struct dt_node *node);

/* bus.c */
state dt_address(const struct dt_node *node, int index, resource_size_t *addr, resource_size_t *size);
int dt_address_nr(const struct dt_node *node);

#endif
