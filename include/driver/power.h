/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_POWER_H_
#define _DRIVER_POWER_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>

/**
 * struct power_device - Describe a power supply.
 * @dev: points to the parent device of the power supply.
 * @list: linked list for centralized management of power supply.
 * @ops: operations method of power supply.
 */
struct power_device {
    struct device *dev;
    struct list_head list;
    struct power_ops *ops;
};

/**
 * struct power_ops - Describe the operations of a power supply.
 * @restart: Use the device to reset the power of the cpu.
 * @reset: Use the device to reset the power of the whole machine.
 * @shutdown: Use the device to switch off the power of the whole machine.
 */
struct power_ops {
    void (*restart)(struct power_device *pdev);
    void (*reset)(struct power_device *pdev);
    void (*shutdown)(struct power_device *pdev);
};

#ifndef CONFIG_POWER

static inline void power_restart(void) { }
static inline void power_reset(void) { }
static inline void power_shutdown(void) { }

#else /* !CONFIG_POWER */

extern void power_reset(void);
extern void power_restart(void);
extern void power_shutdown(void);

#endif /* CONFIG_POWER */

extern state power_register(struct power_device *pdev);
extern void power_unregister(struct power_device *pdev);

#endif  /* _DEVICE_POWER_H_ */
