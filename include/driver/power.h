/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_POWER_H_
#define _DRIVER_POWER_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>

struct power_device {
    struct device *dev;
    struct list_head list;
    struct power_ops *ops;
};

struct power_ops {
    void (*restart)(struct power_device *);
    void (*reset)(struct power_device *);
    void (*shutdown)(struct power_device *);
};

#ifndef CONFIG_POWER

#include <asm/proc.h>

static inline void power_restart(void) { }
static inline void power_reset(void) { }
static inline void power_shutdown(void) { }

#else /* !CONFIG_POWER */

extern void power_reset(void);
extern void power_restart(void);
extern void power_shutdown(void);

#endif /* CONFIG_POWER */

extern state power_register(struct power_device *);
extern void power_unregister(struct power_device *);

#endif  /* _DEVICE_POWER_H_ */
