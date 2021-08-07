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

struct dt_attribute {
    const char *name;
    int len;
    const void *value;
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
struct dt_node *dt_find_all(struct dt_node *prev);
struct dt_attribute *dt_attribute_find(const struct dt_node *node, const char *name);
const void *dt_attribute_get(const struct dt_node *node, const char *name);
bool dt_match(const struct dt_device_id *id, const struct dt_node *node);
int dt_address_nr(const struct dt_node *node);

static inline bool dt_attribute_read_bool(const struct dt_node *node, 
                                          const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, name);
    return !!attribute;
}

#endif
