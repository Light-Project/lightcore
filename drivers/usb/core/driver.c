/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <export.h>
#include <initcall.h>
#include <device/bus.h>
#include <driver/usb.h>
#include <printk.h>

static __always_inline bool
usb_device_match_one(const struct usb_device_id *id,
                     const struct usb_device_descriptor *descriptor)
{
    return ((id->idVendor == descriptor->idVendor ||
           !(id->match_mode & USB_DEV_MATCH_VENDOR)) &&
            (id->idProduct == descriptor->idProduct ||
           !(id->match_mode & USB_DEV_MATCH_PRODUCT)) &&
            (id->bDeviceClass == descriptor->bDeviceClass ||
           !(id->match_mode & USB_DEV_MATCH_DEV_CLASS)) &&
            (id->bDeviceSubClass == descriptor->bDeviceSubClass ||
           !(id->match_mode & USB_DEV_MATCH_DEV_SUBCLASS)) &&
            (id->bDeviceProtocol == descriptor->bDeviceProtocol ||
           !(id->match_mode & USB_DEV_MATCH_DEV_PROTOCOL)) &&
            (id->bcdDevice_lo < descriptor->bcdDevice ||
           !(id->match_mode & USB_DEV_MATCH_DEV_LO)) &&
            (id->bcdDevice_hi > descriptor->bcdDevice ||
           !(id->match_mode & USB_DEV_MATCH_DEV_HI)));
}

/**
 * usb_device_match - See if a device matches a driver's list of IDs
 */
static const struct usb_device_id *
usb_bus_device_match(struct usb_driver *udrv, struct usb_device *udev)
{
    const struct usb_device_id *pids = udrv->id_table;

    while (pids->idVendor || pids->idProduct || pids->bDeviceClass ||
          pids->bInterfaceClass) {
        if (usb_device_match_one(pids, &udev->descriptor))
            return pids;
        ++pids;
    }

    return NULL;
}

static state usb_bus_match(struct device *dev, struct driver *drv)
{
    struct usb_device *udev = device_to_usb_device(dev);
    struct usb_driver *udrv = driver_to_usb_driver(drv);
    return usb_bus_device_match(udrv, udev) ? -ENOERR : -ENODEV;
}

static state usb_bus_probe(struct device *dev)
{
    struct usb_device *udev = device_to_usb_device(dev);
    struct usb_driver *udrv = driver_to_usb_driver(dev->driver);
    const struct usb_device_id *pid;

    /* If pci driver no probe function, exit */
    if (!udrv->probe)
        return -ENXIO;

    pid = usb_bus_device_match(udrv, udev);
    if (!pid)
        return -ENODEV;

    return udrv->probe(udev, pid->data);
}

static state usb_bus_remove(struct device *dev)
{
    struct usb_device *udev = device_to_usb_device(dev);
    struct usb_driver *udrv = driver_to_usb_driver(dev->driver);

    if (!udrv->remove)
        return -ENXIO;

    return udrv->remove(udev);
}

struct bus_type usb_bus_type = {
    .name = "usb",
    .match = usb_bus_match,
    .probe = usb_bus_probe,
    .remove = usb_bus_remove,
};

static state usb_bus_init(void)
{
    /* Register pci bus to system */
    return bus_register(&usb_bus_type);
}
framework_initcall(usb_bus_init);

/**
 * usb_driver_register - Register a new pci device
 * @drv: the pci driver struct to register
 */
state usb_driver_register(struct usb_driver *udrv)
{
    udrv->driver.bus = &usb_bus_type;
    return driver_register(&udrv->driver);
}
EXPORT_SYMBOL(usb_driver_register);

void usb_driver_unregister(struct usb_driver *udrv)
{
    driver_unregister(&udrv->driver);
}
EXPORT_SYMBOL(usb_driver_unregister);
