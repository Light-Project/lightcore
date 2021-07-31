/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCE_H_
#define _DRIVER_CLOCKSOURCE_H_

#include <state.h>

struct clocksource_device;

struct clocksource_ops {
    state (*enable)(struct clocksource_device *);
    state (*disable)(struct clocksource_device *); 
};

struct clocksource_device {
    struct clocksource_ops *ops;
};

state clocksource_register(struct clocksource_device *);

void timer_init(void);

#endif /* _DRIVER_CLOCKSOURCE_H_ */
