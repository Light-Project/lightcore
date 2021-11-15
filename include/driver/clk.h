/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLK_
#define _DRIVER_CLK_

#include <types.h>
#include <state.h>
#include <driver/dt.h>

struct clk_channel {
    const char *name;
    struct clk_device *dev;
};

struct clk_device {
    struct clk_ops *ops;
};

struct clk_ops {
    state (*enable)(struct clk_device *, unsigned int channel);
    state (*disable)(struct clk_device *, unsigned int channel);
    state (*parent_get)(struct clk_device *, unsigned int channel, unsigned int *index);
    state (*parent_set)(struct clk_device *, unsigned int channel, unsigned int index);
    state (*rate_get)(struct clk_device *, unsigned int channel, uint64_t parent, uint64_t *rate);
    state (*rate_set)(struct clk_device *, unsigned int channel, uint64_t parent, uint64_t rate);
};

#ifndef CONFIG_CLK

static inline uint64_t clk_rate_get(struct clk_channel *channel)
{
    return 0;
}

static inline void clk_init(void)
{

}

#else

extern struct clk_channel *dt_get_clk_channel(struct dt_node *, unsigned int index);
extern struct clk_channel *dt_get_clk_channel_by_name(struct dt_node *, const char *name);

extern state clk_enable(struct clk_channel *channel);
extern void clk_disable(struct clk_channel *channel);
extern uint64_t clk_rate_get(struct clk_channel *channel);
extern state clk_rate_set(struct clk_channel *channel, uint64_t rate);

extern state clk_register(struct clk_device *);
extern void clk_unregister(struct clk_device *);
extern void clk_init(void);

#endif  /* CONFIG_CLK */
#endif  /* _DRIVER_CLK_ */
