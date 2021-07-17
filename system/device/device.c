#include <device.h>
#include <printk.h>

/**
 * device_bind_driver - Bind single device and driver through bus
 *
 */
static bool device_bind_driver(struct device *dev, struct driver *drv)
{
    struct bus_type *bus = dev->bus;
    state ret;

    ret = bus->match(dev, drv);
    if(ret == -ENODEV)
        return false;

    dev->driver = drv;
    return true;
}

state device_bind(struct device *device)
{
    struct driver *drv;
    bool ret;

    /* already */
    if(device->driver)
        return -ENOERR;

    bus_for_each_driver(drv, device->bus)
        if((ret = device_bind_driver(device, drv)))
            break;
    
    if(!ret)
        return -ENODEV;

    bus_probe_device(device);

    return -ENOERR;
}
