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

static inline void __noreturn power_restart(void)
{
    proc_reset();
}

static inline void __noreturn power_reset(void)
{
    power_restart();
}

static inline void __noreturn power_shutdown(void)
{
    proc_halt();
}

#else /* !CONFIG_POWER */

void __noreturn power_reset(void);
void __noreturn power_restart(void);
void __noreturn power_shutdown(void);

#endif /* CONFIG_POWER */

state power_register(struct power_device *);
void power_unregister(struct power_device *);

#endif  /* _DEVICE_POWER_H_ */
