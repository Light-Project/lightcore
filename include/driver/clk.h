/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLK_
#define _DRIVER_CLK_

#include <types.h>
#include <state.h>

struct clk_channel {

    struct clk_device *dev;
};

struct clk_device {
    struct clk_ops *ops;
};

struct clk_ops {
    state (*enable)(struct clk_channel *);
    state (*disable)(struct clk_channel *);
};

state clk_channel_enable(struct clk_channel *);
state clk_channel_disable(struct clk_channel *);

#endif  /* _DRIVER_CLK_ */
