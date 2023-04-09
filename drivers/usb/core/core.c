/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "usb"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
#include <driver/usb.h>
#include <printk.h>

MUTEX_LOCK(usb_host_lock);

state usb_host_request_submit(struct usb_request *request)
{
    struct usb_host *host = usb_bus_to_host(request->device->bus);
    state ret;

    if (!host->ops->enqueue)
        return -ENXIO;

    ret = host->ops->enqueue(host, request);
    return ret;
}

struct usb_device *usb_device_alloc(struct usb_bus *bus, struct usb_device *parent)
{
    struct usb_device *udev;

    udev = kzalloc(sizeof(*udev), GFP_KERNEL);
    if (!udev)
        return NULL;

    udev->bus = bus;
    udev->device.bus = &usb_bus_type;
    return udev;
}

state usb_device_register(struct usb_device *udev)
{
    device_register(&udev->device);
    return -ENOERR;
}

static state root_hub_register(struct usb_host *host)
{
    struct usb_device *root = host->bus.root_hub;

    root->devnr = 1;
    usb_descriptor_get(root, 0);

    return -ENOERR;
}

state usb_host_register(struct usb_host *host)
{
    struct usb_device *root_hub;
    state ret;

    if (!host->dev || !host->ops)
        return -EINVAL;

    root_hub = usb_device_alloc(&host->bus, NULL);
    host->bus.root_hub = root_hub;

    switch (host->type) {
        case USB_HOST_11:
            root_hub->speed = USB_SPEED_FULL;
            break;
        case USB_HOST_2:
            root_hub->speed = USB_SPEED_HIGH;
            break;
        case USB_HOST_25:
            root_hub->speed = USB_SPEED_WIRELESS;
            break;
        case USB_HOST_3:
            root_hub->speed = USB_SPEED_SUPER;
            break;
        case USB_HOST_31:
            root_hub->speed = USB_SPEED_SUPER_PLUS;
            break;
        case USB_HOST_32:
            root_hub->speed = USB_SPEED_SUPER_PLUS_2;
            break;
        default:
            return -EINVAL;
    }

    if (host->ops->setup) {
        ret = host->ops->setup(host);
        if (unlikely(ret)) {
            dev_err(host->dev, "reset error\n");
            return -EBUSY;
        }
    }

    ret = host->ops->start(host);
    if (unlikely(ret)) {
        dev_err(host->dev, "start error\n");
        return -EBUSY;
    }

    root_hub_register(host);

    return -ENOERR;
}
