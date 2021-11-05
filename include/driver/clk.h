/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLK_
#define _DRIVER_CLK_

#include <types.h>
#include <state.h>
#include <driver/dt.h>

struct clk_channel {
    struct clk_device *dev;
};

struct clk_device {
    struct clk_ops *ops;
};

struct clk_ops {
    state (*enable)(struct clk_device *);
    state (*disable)(struct clk_device *);

    uint64_t (*get_freq)(struct clk_device *, uint64_t parent);
    state (*set_freq)(struct clk_device *, uint64_t parent, uint64_t freq);
};

#ifndef CONFIG_CLK

static inline state clk_register(struct clk_device *clk)
{
    return -ENOERR;
}

static inline void clk_unregister(struct clk_device *clk) {}
static inline void clk_init(void) {}

#else

struct clk_channel *dt_get_clk_channel(struct dt_node *, unsigned int index);

state clk_enable(struct clk_channel *);
void clk_disable(struct clk_channel *);

state clk_register(struct clk_device *);
void clk_unregister(struct clk_device *);
void clk_init(void);

#endif  /* CONFIG_CLK */
#endif  /* _DRIVER_CLK_ */
