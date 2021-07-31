/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com> 
 */

#define DRIVER_NAME "ehci-pci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <mm.h>
#include <init/initcall.h>
#include <driver/pci.h>
#include <driver/usb.h>
#include <printk.h>

static struct pci_device_id ehci_pci_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_SERIAL_USB_EHCI, ~0) },
    { }, /* NULL */
};

static state ehci_probe(struct pci_device *pdev, int data)
{
    struct usb_host *host;
    resource_size_t start, size;

    host = kzalloc(sizeof(*host), GFP_KERNEL);
    if(!host)
        return -ENOMEM;

    start = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);
    host->regs = ioremap(start, size);
    if(!host->regs)
        return -ENOMEM;


    return -ENOERR;
}

static state ehci_remove(struct pci_device *pdev)
{

    return -ENOERR;
}

static struct pci_driver ehci_pci_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = ehci_pci_ids,
    .probe = ehci_probe,
    .remove = ehci_remove,
};
 
static state ehci_pci_init(void)
{
    return pci_driver_register(&ehci_pci_driver);
}
driver_initcall(ehci_pci_init);
