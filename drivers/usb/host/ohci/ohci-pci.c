/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <delay.h>
#include <driver/pci.h>

static void uhci_pci_reset(struct uhci_host *uhci, struct pci_device *pdev)
{
    /* Turn off PIRQ enable and SMI enable.  (This also turns off the
     * BIOS's USB Legacy Support.)  Turn off all the R/WC bits too.
     */
    pci_config_writew(pdev, UHCI_PCI_LEGSUP, UHCI_PCI_LEGSUP_RWC);

    /* Reset the HC - this will force us to get a
     * new notification of any already connected
     * ports due to the virtual disconnect that it
     * implies.
     */
	outw(uhci->port + UHCI_CMD, UHCI_CMD_HCRESET);
    mb();
	udelay(5);

	outw(uhci->port + UHCI_INTR, 0);
	outw(uhci->port + UHCI_CMD, 0);
}

static state uhci_pci_setup(struct usb_host *host)
{
    struct uhci_host *uhci = host_to_uhci(host);
    struct pci_device *pdev = device_to_pci_device(uhci->device);
    uint16_t val;

    val = pci_config_readw(pdev, UHCI_PCI_LEGSUP);
    if (val & ~(UHCI_PCI_LEGSUP_RO | UHCI_PCI_LEGSUP_RWC)) {
        goto reset;
    }

	val = inw(uhci->port + UHCI_CMD);
    if ((val & UHCI_CMD_RUN) || !(val & UHCI_CMD_CF) ||
       !(val & UHCI_CMD_EGSM)) {
        goto reset;
    }

	val = inw(uhci->port + UHCI_INTR);
    if (val & ~UHCI_CMD_RUN) {
        goto reset;
    }

    return -ENOERR;

reset:
    uhci_pci_reset(uhci, pdev);
    return -ENOERR;
}

static struct usb_ops uhci_ops = {
    .setup = uhci_pci_setup,
    .start = uhci_start,
    .ep_disable = uhci_endpoint_disable,

    .enqueue = uhci_enqueue,
};

static struct pci_device_id uhci_pci_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_SERIAL_USB_OHCI, ~0) },
    { }, /* NULL */
};

static state uhci_pci_probe(struct pci_device *pdev, int data)
{
    struct uhci_host *uhci;
    int val;

    uhci = kzalloc(sizeof(*uhci), GFP_KERNEL);
    if (!uhci)
        return -ENOMEM;

    for (val = 0; val < PCI_STD_NUM_BARS; ++val) {
        if (pci_resource_type(pdev, val) != RESOURCE_PMIO)
            continue;
        uhci->port = pci_resource_start(pdev, 0);
        break;
    } if (val == PCI_ROM_RESOURCE) {
        pr_warn("no i/o available\n");
        return -ENODEV;
    }

    irq_request(pdev->irq, 0, uhci_handle, uhci, "uhci-pci");

    uhci->device = &pdev->dev;
    uhci->host.type = USB_HOST_11;
    uhci->host.ops  = &uhci_ops;
    return usb_host_register(&uhci->host);
}

static state uhci_pci_remove(struct pci_device *pdev)
{

    return -ENOERR;
}

static struct pci_driver uhci_pci_driver = {
    .driver = {
        .name = "uhci-pci",
    },
    .id_table = uhci_pci_ids,
    .probe = uhci_pci_probe,
    .remove = uhci_pci_remove,
};

static state uhci_pci_init(void)
{
    return pci_driver_register(&uhci_pci_driver);
}
driver_initcall(uhci_pci_init);
