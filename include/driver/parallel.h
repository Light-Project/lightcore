/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PARALLEL_H_
#define _DRIVER_PARALLEL_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>

enum parallel_flags {
    PARALLEL_NONE,
};

struct parallel_device {
    struct device *dev;
    struct list_head list;
    struct parallel_ops *ops;
};

struct parallel_ops {
    state (*data_read)(struct parallel_device *, uint8_t *data);
    state (*data_write)(struct parallel_device *, uint8_t data);

    state (*epp_data_read)(struct parallel_device *, const void *buff,
                        unsigned int len, enum parallel_flags flags);
    state (*epp_data_write)(struct parallel_device *, void *buff,
                        unsigned int len, enum parallel_flags flags);
    state (*epp_addr_read)(struct parallel_device *, const void *buff,
                        unsigned int len, enum parallel_flags flags);
    state (*epp_addr_write)(struct parallel_device *, void *buff,
                        unsigned int len, enum parallel_flags flags);

    state (*ecp_data_read)(struct parallel_device *, const void *buff,
                        unsigned int len, enum parallel_flags flags);
    state (*ecp_data_write)(struct parallel_device *, void *buff,
                        unsigned int len, enum parallel_flags flags);
    state (*ecp_addr_write)(struct parallel_device *, void *buff,
                        unsigned int len, enum parallel_flags flags);
};

state parallel_register(struct parallel_device *);
void parallel_unregister(struct parallel_device *);

#endif  /* _DRIVER_PARALLEL_H_ */

