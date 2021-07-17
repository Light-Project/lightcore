/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_PLATFORM_H_
#define _DEVICE_PLATFORM_H_

#include <device.h>
#include <resource.h>
#include <mod_devicetable.h>

extern struct bus_type platform_bus;

struct platform_device {

    /* Bus structure description */
    const char *name;               /* Match witch driver */
    struct device device;           /* Generic device */

    int         id;
    bool        id_auto;
    uint64_t    platform_dma_mask;
    uint32_t    num_resources;
};

#define device_to_platform_device(dev)	\
        (container_of((dev), struct platform_device, device))

static inline void *platform_get_devdata(const struct platform_device *pdev)
{
	return dev_get_devdata((const struct device *)&pdev->device);
}

static inline void platform_set_devdata(struct platform_device *pdev, void *data)
{
	return dev_set_devdata((struct device *)&pdev->device, data);
}

extern state platform_device_register(struct platform_device *);
extern void platform_device_unregister(struct platform_device *);

struct platform_driver {
    struct driver driver;
    const struct dt_device_id   *dt_table;
    const struct acpi_device_id *acpi_table;

    state (*probe)(struct platform_device *pdev);
    state (*remove)(struct platform_device *pdev);
    void  (*shutdown)(struct platform_device *pdev);
    state (*suspend)(struct platform_device *pdev, pm_message_t state);
    state (*resume)(struct platform_device *pdev);
};

#define driver_to_platform_driver(drv)	\
        (container_of((drv), struct platform_driver, driver))

extern state platform_driver_register(struct platform_driver *);
extern void platform_driver_unregister(struct platform_driver *);

void platform_bus_init(void);

#endif  /* _DEVICE_PLATFORM_H_ */
