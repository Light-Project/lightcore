/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "i2c"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <kmalloc.h>
#include <driver/platform.h>
#include <driver/i2c.h>
#include <printk.h>
#include <export.h>

static struct kcache *i2c_device_cache;
SPIN_LOCK(i2c_host_lock);
LIST_HEAD(i2c_host_list);

const char *i2c_speed_mode_string(unsigned int speed)
{
    if (speed <= I2C_SPEED_STANDARD)
        return "Standard Mode (100 kHz)";
    else if (speed <= I2C_SPEED_FAST)
        return "Fast Mode (400 kHz)";
    else if (speed <= I2C_SPEED_FAST_PLUS)
        return "Fast Mode Plus (1.0 MHz)";
    else if (speed <= I2C_SPEED_TURBO)
        return "Turbo Mode (1.4 MHz)";
    else if (speed <= I2C_SPEED_HIGH_SPEED)
        return "High Speed Mode (3.4 MHz)";
    else if (speed <= I2C_SPEED_ULTRA_FAST)
        return "Ultra Fast Mode (5.0 MHz)";
    else /* speed > I2C_SPEED_ULTRA_FAST */
		return "Unknown Mode";
}
EXPORT_SYMBOL(i2c_speed_mode_string);

static state i2c_match(struct device *dev, struct driver *drv)
{
    struct i2c_device *idev = device_to_i2c_device(dev);
    struct i2c_driver *idrv = driver_to_i2c_driver(drv);
    const struct platform_device_id *platform;

#ifdef CONFIG_PARALLEL_DT
    const struct dt_device_id *dt;
    dt = platform_dt_match(idrv->dt_table, idev->dt_node);
    if (dt) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)dt->data);
        return -ENOERR;
    }
#endif

    platform = platform_device_match(idrv->platform_table, idev->name);
    if (platform) {
        if (!device_get_pdata(dev))
            device_set_pdata(dev, (void *)platform->data);
        return -ENOERR;
    }

    return -ENODEV;
}

static state i2c_probe(struct device *dev)
{
    struct i2c_device *idev = device_to_i2c_device(dev);
    struct i2c_driver *idrv = driver_to_i2c_driver(dev->driver);

    if (unlikely(!idrv->probe)) {
        pr_warn("probe not supported\n");
        return -ENXIO;
    }

    return idrv->probe(idev, device_get_pdata(dev));
}

static state i2c_remove(struct device *dev)
{
    struct i2c_device *idev = device_to_i2c_device(dev);
    struct i2c_driver *idrv = driver_to_i2c_driver(dev->driver);

    if (!idrv->remove)
        return -ENXIO;

    idrv->remove(idev);
    return -ENOERR;
}

static state i2c_shutdown(struct device *dev)
{
    struct i2c_device *idev = device_to_i2c_device(dev);
    struct i2c_driver *idrv = driver_to_i2c_driver(dev->driver);

    if (!idrv->shutdown)
        return -ENXIO;

    idrv->shutdown(idev);
    return -ENOERR;
}

struct bus_type i2c_bus = {
    .name = "i2c",
    .match = i2c_match,
    .probe = i2c_probe,
    .remove = i2c_remove,
    .shutdown = i2c_shutdown,
};

/**
 * i2c_device_alloc - allocating for a new i2c device.
 * @host: parent host controller of the device.
 * @name: device name for matching.
 */
struct i2c_device *i2c_device_alloc(struct i2c_host *host, const char *name)
{
    struct i2c_device *idev;

    idev = kcache_zalloc(i2c_device_cache, GFP_KERNEL);
    if (!idev)
        return NULL;

    idev->name = name;
    idev->host = host;

    return idev;
}
EXPORT_SYMBOL(i2c_device_alloc);

/**
 * i2c_device_free - release a new i2c device.
 * @idev: the device to be release.
 */
void i2c_device_free(struct i2c_device *idev)
{
    kcache_free(i2c_device_cache, idev);
}
EXPORT_SYMBOL(i2c_device_free);

/**
 * i2c_device_register - register a device to the i2c bus.
 * @idev: registering i2c device.
 */
state i2c_device_register(struct i2c_device *idev)
{
    idev->dev.bus = &i2c_bus;
    return device_register(&idev->dev);
}
EXPORT_SYMBOL(i2c_device_register);

/**
 * i2c_device_unregister - unregister a device to the i2c bus.
 * @idev: unregistering i2c device.
 */
void i2c_device_unregister(struct i2c_device *idev)
{
    device_unregister(&idev->dev);
}
EXPORT_SYMBOL(i2c_device_unregister);

/**
 * i2c_driver_register - register a driver to the i2c bus.
 * @idrv: registering i2c driver.
 */
state i2c_driver_register(struct i2c_driver *idrv)
{
    idrv->driver.bus = &i2c_bus;
    return driver_register(&idrv->driver);
}
EXPORT_SYMBOL(i2c_driver_register);

/**
 * i2c_driver_unregister - unregister a driver to the i2c bus.
 * @idrv: unregistering i2c driver.
 */
void i2c_driver_unregister(struct i2c_driver *idrv)
{
    driver_unregister(&idrv->driver);
}
EXPORT_SYMBOL(i2c_driver_unregister);

state i2c_host_register(struct i2c_host *host)
{
    if (!host->dev || !host->ops)
        return -EINVAL;

#ifdef CONFIG_I2C_DT
    dt_populate_i2c(host);
#endif

    spin_lock(&i2c_host_lock);
    list_add(&i2c_host_list, &host->list);
    spin_unlock(&i2c_host_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(i2c_host_register);

void i2c_host_unregister(struct i2c_host *host)
{
    spin_lock(&i2c_host_lock);
    list_del(&host->list);
    spin_unlock(&i2c_host_lock);
}
EXPORT_SYMBOL(i2c_host_unregister);

static state i2c_init(void)
{
    i2c_device_cache = kcache_create(
        "i2c device",
        sizeof(struct i2c_device),
        KCACHE_PANIC
    );

    return bus_register(&i2c_bus);
}
framework_initcall(i2c_init);
