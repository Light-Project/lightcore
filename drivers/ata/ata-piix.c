/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <types.h>
#include <state.h>
#include <driver/pci.h>
#include <initcall.h>
#include <driver/ata.h>
#include <printk.h>

// static state piix_probe(struct pci_device *pdev, const void *pdata)
// {
//     return -ENOERR;
// }

// static struct pci_driver piix_pci_driver = {
//     .driver = {
//         .name = "ata-piix",
//     },
//     .probe = piix_probe,
// };

// static state piix_ata_init(void)
// {
//     return pci_driver_register(&piix_pci_driver);
// }
// driver_initcall(piix_ata_init);
