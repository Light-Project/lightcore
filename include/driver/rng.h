/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RNG_H_
#define _DRIVER_RNG_H_

#include <device.h>
#include <rating.h>

/**
 * struct rng_device - describe a rng device.
 * @dev: points to the parent device of the rng device.
 * @ops: operations method of rng device.
 * @list: linked list for centralized management of rng device.
 * @rating: speed at which rng generates true random numbers.
 * @pdata: private data of rng device.
 */
struct rng_device {
    struct list_head list;
    struct device *dev;
    struct rng_ops *ops;
    unsigned int rating;
    void *pdata;
};

/**
 * struct rng_ops - describe the operations of a rng.
 * @enable: enable the rng device.
 * @disable: disable the rng device.
 * @read: read random values from rng device.
 * @filled: fill array with random values from rng device.
 */
struct rng_ops {
    state (*enable)(struct rng_device *rdev);
    state (*disable)(struct rng_device *rdev);
    state (*read)(struct rng_device *rdev, unsigned long *data);
    state (*filled)(struct rng_device *rdev, void *data, size_t len);
};

extern spinlock_t rng_lock;
extern struct list_head rng_list;

extern state rng_enable(struct rng_device *rdev);
extern state rng_disable(struct rng_device *rdev);
extern state rng_read(struct rng_device *rdev, unsigned long *data);
extern state rng_filled(struct rng_device *rdev, void *data, size_t len);

extern state rng_register(struct rng_device *rdev);
extern void rng_unregister(struct rng_device *rdev);

#endif  /* _DRIVER_RNG_H_ */
