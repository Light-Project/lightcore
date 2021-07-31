/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "usb-hub"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <init/initcall.h>
#include <driver/usb.h>

static state hub_probe(struct usb_device *udev, int id)
{
    
}

static state hub_remove(struct usb_device *udev)
{
    
}

static struct usb_device_id hub_id_table[] = {
    { USB_DEVICE_CLASS(USB_CLASS_HUB) },
    { USB_INT_CLASS(USB_CLASS_HUB) },
    { }, /* NULL */
};

static struct usb_driver hub_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = hub_id_table,
    .probe = hub_probe,
    .remove = hub_remove,
};

state usb_hub_init(void)
{
    return usb_driver_register(&hub_driver);
}
driver_initcall(usb_hub_init);
