#ifndef _DRIVER_DT_H_
#define _DRIVER_DT_H_

#include <state.h>
#include <slist.h>
#include <system/kobject.h>
#include <mod_devicetable.h>

struct dt_node {
    const char *name;           /* node name */
    slist_t sibling;
    struct dt_node  *parent;
    struct dt_node  *child;
    struct kobject kobj;
    void *driver_data;
};

extern struct dt_node *dt_root;
extern struct dt_node *dt_chosen;
extern struct dt_node *dt_stdout;

struct dt_node *dt_next_child(const struct dt_node *parent, const struct dt_node *prev);

#define dt_for_each_child_node(bus, child)      \
        for(child = dt_next_child(bus, NULL);   \
            child != NULL;                      \
            child = dt_next_child(bus, child))


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

state dt_scan_node(state (*fn)(unsigned long node, const char *uname, int depth, void *data), void *data);
const void *dt_get_prop(unsigned long node, const char *name, int *size);


state early_dt_scan(void *dt_start);

#endif
