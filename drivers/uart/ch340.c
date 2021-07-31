/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ch340"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <init/initcall.h>
#include <driver/usb.h>
#include <driver/uart/ch340.h>

// static __always_inline state
// ch340_control_in(struct usb_device *udev, uint8_t req)
// {

// }

// static __always_inline state
// ch340_control_out(struct usb_device *udev, uint8_t req)
// {
    
// }

static struct usb_device_id ch340_id_table[] = {
    { .idVendor = 0x1a86, .idProduct = 0x5512 },
    { .idVendor = 0x1a86, .idProduct = 0x5523 },
    { .idVendor = 0x1a86, .idProduct = 0x7522 },
    { .idVendor = 0x1a86, .idProduct = 0x7523 },
    { .idVendor = 0x4348, .idProduct = 0x5523 },
    { .idVendor = 0x9986, .idProduct = 0x7523 },
    { }, /* NULL */
};

static state ch340_probe(struct usb_device *udev, int id)
{

    return -ENOERR;
}
static state ch340_remove(struct usb_device *udev)
{

    return -ENOERR;
}

static struct usb_driver ch341_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = ch340_id_table,
    .probe = ch340_probe,
    .remove = ch340_remove,
};

static state ch340_init(void)
{
    return usb_driver_register(&ch341_driver);
}
driver_initcall(ch340_init);
