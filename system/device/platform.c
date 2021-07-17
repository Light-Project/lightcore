/* SPDX-License-Identifier: GPL-2.0-or-later */
/**
 * Platform is the root device bus of the kernel 
 */

#include <driver/platform.h> 
#include <init/initcall.h>
#include <string.h>

state platform_device_register(struct platform_device *pdev)
{
    pdev->device.bus = &platform_bus;
    return device_bind(&pdev->device);
}

void platform_device_unregister(struct platform_device *pdev)
{

}

state platform_driver_register(struct platform_driver *pdrv)
{
    pdrv->driver.bus = &platform_bus;
    return driver_register(&pdrv->driver);
}

void platform_driver_unregister(struct platform_driver *pdrv)
{
    driver_unregister(&pdrv->driver);
}

static state platform_match(struct device *dev, struct driver *drv)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    bool ret;

    ret = strcmp(drv->name, pdev->name);
    if(!ret)
        return -ENOERR;

    return -ENODEV;
}

static state platform_probe(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->probe)
        return -ENODEV;

    return pdrv->probe(pdev);
}

static state platform_remove(struct device *dev)
{
    struct platform_device *pdev = device_to_platform_device(dev);
    struct platform_driver *pdrv = driver_to_platform_driver(dev->driver);

    if (!pdrv->remove)
        return -ENODEV;

    return pdrv->remove(pdev);
}

struct bus_type platform_bus = {
    .name = "platform",
    .match = platform_match,
    .probe = platform_probe,
    .remove = platform_remove,
};

void __init platform_bus_init(void)
{
    bus_register(&platform_bus);
}
