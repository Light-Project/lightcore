/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>

void pci_irq_setup(struct pci_device *pdev)
{
    struct pci_host *host;
    uint8_t pin, slot = -1;
    unsigned int irq = 0;

    host = pci_bus_find_host(pdev->bus);
    if (!(host->ops->irq_map))
        return;

    pin = pci_config_readb(pdev, PCI_INTERRUPT_LINE);
    if (pin > 4)
        pin = 1;

    if (pin) {
        if (host->ops->irq_swizzle)
			slot = host->ops->irq_swizzle(pdev, &pin);

		irq = host->ops->irq_map(pdev, slot, pin);
		if (irq == -1)
			irq = 0;
    }

    pdev->irq = irq;
    pci_debug(pdev, "setup IRQ: got %d\n", irq);
    pci_config_writeb(pdev, PCI_INTERRUPT_LINE, irq);
}
