/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RNG_H_
#define _DRIVER_RNG_H_

#include <device.h>

struct rng_device {
    struct list_head list;
    struct device *dev;
    struct rng_ops *ops;
    unsigned int rating;
};

struct rng_ops {
    state (*open)(struct rng_device *);
    void (*close)(struct rng_device *);
    state (*read)(struct rng_device *, unsigned long *data);
};

extern state rng_register(struct rng_device *);
extern void rng_unregister(struct rng_device *);

#endif  /* _DRIVER_RNG_H_ */
