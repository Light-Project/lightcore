/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_ISA_H_
#define _DRIVER_ISA_H_

#include <device.h>

/**
 * struct isa_device - describe a isa device.
 * @dev: kernel infrastructure device architecture.
 * @name: name used to match the platform_table.
 * @dt_node: device tree node of the device.
 * @host: parallel controller used with the device.
 */
struct isa_device {
	struct device dev;
	struct list_head list;
	unsigned int index;
};

/**
 * struct isa_driver - describe a isa driver.
 * @driver: kernel infrastructure driver architecture.
 * @probe: binds this driver from the parallel device.
 * @remove: unbinds this driver from the parallel device.
 * @shutdown: shutdown parallel device.
 */
struct isa_driver {
    struct driver driver;
	struct list_head devices;

	state (*match)(struct isa_device *idev, unsigned int index);
	state (*probe)(struct isa_device *idev, unsigned int index, const void *pdata);
	void (*remove)(struct isa_device *idev, unsigned int index);
	void (*shutdown)(struct isa_device *idev, unsigned int index);
	state (*suspend)(struct isa_device *idev, unsigned int index, pm_message_t msg);
	state (*resume)(struct isa_device *idev, unsigned int index);
};

#define device_to_isa_device(ptr) \
    container_of(ptr, struct isa_device, dev)

#define driver_to_isa_driver(ptr) \
    container_of(ptr, struct isa_driver, driver)

static inline void *isa_get_devdata(const struct isa_device *pdev)
{
    return device_get_pdata(&pdev->dev);
}

static inline void isa_set_devdata(struct isa_device *pdev, void *data)
{
    return device_set_pdata(&pdev->dev, data);
}

#ifndef CONFIG_ISA_BUS
static inline state isa_driver_register(struct isa_driver *idrv, unsigned int num, const void *pdata)
{
	return -ENODEV;
}

static inline void isa_driver_unregister(struct isa_driver *idrv)
{
}
#else
extern state isa_driver_register(struct isa_driver *idrv, unsigned int num, const void *pdata);
extern void isa_driver_unregister(struct isa_driver *idrv);
#endif

#endif  /* _DRIVER_ISA_H_ */
