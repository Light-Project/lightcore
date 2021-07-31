/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_H_
#define _DRIVER_DT_H_

#include <state.h>
#include <slist.h>
#include <kernel/kobject.h>
#include <mod_devicetable.h>

extern void *dt_start_addr;
extern uint32_t dt_crc32;

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

#define _DT_DECLARE(table, name, compat, fn, fn_type)   \
    static const struct dt_device_id __dt_table_##name  \
        __used __section("__" #table "_dt_table")       \
        __aligned(__alignof__(struct dt_device_id))     \
        = { .compatible = compat,                       \
            .data = (fn == (fn_type)NULL) ? fn : fn  }
        
#ifdef CONFIG_FRAMEWORK_DT
#define DT_DECLARE(table, name, compat, fn, fn_type)    \
        _DT_DECLARE (table, name, compat, fn, fn_type)
#else
#define DT_DECLARE(table, name, compat, fn, fn_type)
#endif
        
typedef void (* dt_init_fn_1)(struct dt_node *);
typedef int (* dt_init_fn_1_ret)(struct dt_node *);
typedef int (* dt_init_fn_2)(struct dt_node *, struct dt_node *);
    
#define DT_DECLARE_1(table, name, compat, fn)       \
        DT_DECLARE(table, name, compat, fn, dt_init_fn_1)
#define DT_DECLARE_1_RET(table, name, compat, fn)   \
        DT_DECLARE(table, name, compat, fn, dt_init_fn_1_ret)
#define DT_DECLARE_2(table, name, compat, fn)       \
        DT_DECLARE(table, name, compat, fn, dt_init_fn_2)

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

/* early.c */
state early_dt_scan(void *dt_start);

/* fdt.c */
struct dt_attribute *dt_find_attribute(const struct dt_node *node, const char *name);
state dt_scan_node(state (*fn)(unsigned long node, const char *uname, int depth, void *data), void *data);
const void *dt_get_prop(unsigned long node, const char *name, int *size);
void dt_init(void);

#endif
