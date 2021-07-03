#ifndef _DEVICE_PLATFORM_H_
#define _DEVICE_PLATFORM_H_

#include <device.h>

extern struct bus_type platform_bus;

struct platform_device {
    const char  *name;
    int         id;
    bool        id_auto;
    struct device    device;       // device 
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
    const struct dt_device_id *dt_table;

    state (*probe)(struct platform_device *);
    state (*remove)(struct platform_device *);
    void (*shutdown)(struct platform_device *);
    state (*suspend)(struct platform_device *, pm_message_t state);
    state (*resume)(struct platform_device *);
};

#define driver_to_platform_driver(drv)	\
        (container_of((drv), struct platform_driver, driver))

extern state platform_driver_register(struct platform_driver *);
extern void platform_driver_unregister(struct platform_driver *);

#endif
