/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <init/initcall.h>
#include <driver/pci.h>


static struct pci_device_id echi_pci_ids[] = {
    {
        .vendor = PCI_ANY_ID,
        .device = PCI_ANY_ID,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
        .class = PCI_CLASS_SERIAL_USB_EHCI,
        .class_mask = 0xffff,
    },
    { } /* NULL */
};

static struct pci_driver echi_pci_driver = {
    .driver = {
        .name = "usb-echi-pci",
    },
    .id_table = echi_pci_ids,
    .probe = echi_pci_probe,
    .remove = echi_pci_remove,
};
 
static state echi_pci_init(void)
{
    return pci_device_register(&echi_pci_driver);
}
 
driver_initcall(echi_pci_init);
