#include <driver/dt.h>
#include <driver/dt/libfdt.h>

struct dt_node *dt_root;
struct dt_node *dt_chosen;
struct dt_node *dt_stdout;

struct dt_node *dt_next_child(const struct dt_node *parent, 
                        const struct dt_node *prev)
{
    const struct dt_node *next;

    next = prev ? slist_next_entry(prev, sibling) : parent->child;

    return (struct dt_node *)next;
}
