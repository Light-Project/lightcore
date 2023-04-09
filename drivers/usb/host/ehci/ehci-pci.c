/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>

static state ehci_probe(struct pci_device *pdev, const void *pdata)
{
    struct ehci_host *ehci;

    if (pci_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENXIO;

    ehci = dev_kzalloc(&pdev->dev, sizeof(*ehci), GFP_KERNEL);
    if (!ehci)
        return -ENOMEM;

    ehci->base = pci_resource_ioremap(pdev, 0, 0);
    if (!ehci->base)
        return -ENOMEM;

    ehci->device = &pdev->dev;
    ehci->host.type = USB_HOST_2;
    ehci->host.ops  = &ehci_pci_ops;
    pci_set_devdata(pdev, ehci);

    irq_request(pdev->irq, 0, ehci_irq, ehci, "ehci-pci");
    return usb_host_register(&ehci->host);
}

static struct pci_device_id ehci_pci_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_SERIAL_USB_EHCI, ~0) },
    { }, /* NULL */
};

static struct pci_driver ehci_pci_driver = {
    .driver = {
        .name = "ehci-pci",
    },
    .id_table = ehci_pci_ids,
    .probe = ehci_probe,
};

static state ehci_pci_init(void)
{
    return pci_driver_register(&ehci_pci_driver);
}
driver_initcall(ehci_pci_init);
