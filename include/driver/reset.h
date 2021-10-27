/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RESET_
#define _DRIVER_RESET_

#include <device.h>

struct reset_channel {
    unsigned int index;
    struct reset_device *reset;
    struct list_head list;
};

struct reset_device {
    struct list_head list;
    struct list_head channel_list;
    struct dt_node *dt;
    struct reset_ops *ops;
};

struct reset_ops {
    state (*status)(struct reset_device *, unsigned int channel);
    state (*assert)(struct reset_device *, unsigned int channel);
    state (*deassert)(struct reset_device *, unsigned int channel);
    state (*reset)(struct reset_device *, unsigned int channel);
};

extern struct list_head reset_list;

#ifndef CONFIG_RESET

static inline state reset_status(struct reset_channel *rst)
{
    return -ENOERR;
}

static inline state reset_assert(struct reset_channel *rst)
{
    return -ENOERR;
}

static inline state reset_deassert(struct reset_channel *rst)
{
    return -ENOERR;
}

static inline state reset_reset(struct reset_channel *rst)
{
    return -ENOERR;
}

#else

state reset_status(struct reset_channel *);
state reset_assert(struct reset_channel *);
state reset_deassert(struct reset_channel *);
state reset_reset(struct reset_channel *);

extern struct reset_channel *dt_get_reset_channel(struct dt_node *, unsigned int index);
extern struct reset_channel *dt_get_reset_channel_by_name(struct dt_node *node, const char *name);

extern struct reset_channel *reset_channel_get(struct reset_device *, unsigned int);
extern void reset_channel_release(struct reset_channel *);
extern state reset_register(struct reset_device *);
extern void reset_unregister(struct reset_device *);

#endif  /* CONFIG_RESET */
#endif  /* _DRIVER_RESET_ */
