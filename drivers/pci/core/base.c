/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

#define PCI_CAP_FIND_TTL	48

uint8_t pci_bus_config_readb(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 1, &val);

    return val;
}
EXPORT_SYMBOL(pci_bus_config_readb);

uint16_t pci_bus_config_readw(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 2, &val);

    return val;
}
EXPORT_SYMBOL(pci_bus_config_readw);

uint32_t pci_bus_config_readl(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 4, &val);

    return val;
}
EXPORT_SYMBOL(pci_bus_config_readl);

void pci_bus_config_writeb(struct pci_bus *bus, unsigned int devfn,
                                    unsigned int reg, uint8_t val)
{
    bus->ops->write(bus, devfn, reg, 1, val);
}
EXPORT_SYMBOL(pci_bus_config_writeb);

void pci_bus_config_writew(struct pci_bus *bus, unsigned int devfn,
                                    unsigned int reg, uint16_t val)
{
    bus->ops->write(bus, devfn, reg, 2, val);
}
EXPORT_SYMBOL(pci_bus_config_writew);

void pci_bus_config_writel(struct pci_bus *bus, unsigned int devfn,
                           unsigned int reg, uint32_t val)
{
    bus->ops->write(bus, devfn, reg, 4, val);
}
EXPORT_SYMBOL(pci_bus_config_writel);

uint8_t pci_config_readb(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readb(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readb);

uint16_t pci_config_readw(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readw(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readw);

uint32_t pci_config_readl(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readl(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readl);

void pci_config_writeb(struct pci_device *pdev, unsigned int reg, uint8_t val)
{
    pci_bus_config_writeb(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writeb);

void pci_config_writew(struct pci_device *pdev, unsigned int reg, uint16_t val)
{
    pci_bus_config_writew(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writew);

void pci_config_writel(struct pci_device *pdev, unsigned int reg, uint32_t val)
{
    pci_bus_config_writel(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writel);


static uint8_t pci_capability_find_next_ttl(struct pci_bus *bus, uint16_t devfn,
                                            uint8_t pos, int cap, int ttl)
{
    uint16_t val;
    uint8_t id;

    pos = pci_bus_config_readb(bus, devfn, pos);

    while (ttl--) {
		if (pos < 0x40)
			break;

		pos &= ~3;
        val = pci_bus_config_readw(bus, devfn, pos);
		id = val & 0xff;

		if (id == 0xff)
			break;
		if (id == cap)
			return pos;
        pos = val >> 8;
    }

    return 0;
}

static uint8_t pci_capability_find_start(struct pci_bus *bus,
                                uint16_t devfn, uint8_t head)
{
    uint16_t val = pci_bus_config_readw(bus, devfn, PCI_STATUS);

    if (!(val & PCI_STATUS_CAP_LIST))
        return 0;

    switch (val) {
        case PCI_HEADER_TYPE_NORMAL:
            fallthrough;
        case PCI_HEADER_TYPE_BRIDGE:
            return PCI_CAPABILITY_LIST;
        case PCI_HEADER_TYPE_CARDBUS:
            return PCI_CB_CAPABILITY_LIST;
    }

    return 0;
}

uint8_t pci_capability_find(struct pci_device *pdev, int cap)
{
    uint8_t pos;

    pos = pci_capability_find_start(pdev->bus, pdev->devfn, pdev->head);
    if (!pos)
        return 0;

    return pci_capability_find_next_ttl(pdev->bus, pdev->devfn,
                                    pos, cap, PCI_CAP_FIND_TTL);
}
EXPORT_SYMBOL(pci_capability_find);

state pci_cacheline_size(struct pci_device *pdev)
{
    return -ENOERR;
}
EXPORT_SYMBOL(pci_cacheline_size);

/**
 * pci_mwi_clear - disables Memory-Write-Invalidate for device dev
 * @dev: the PCI device to disable
 *
 * Disables PCI Memory-Write-Invalidate transaction on the device
 */
void pci_mwi_disable(struct pci_device *pdev)
{
	uint16_t cmd;

	cmd = pci_config_readw(pdev, PCI_COMMAND);
	if (cmd & PCI_COMMAND_INVALIDATE) {
		cmd &= ~PCI_COMMAND_INVALIDATE;
		pci_config_writew(pdev, PCI_COMMAND, cmd);
	}
}
EXPORT_SYMBOL(pci_mwi_disable);

state pci_mwi_enable(struct pci_device *pdev)
{
	uint16_t cmd;

	cmd = pci_config_readw(pdev, PCI_COMMAND);
	if (cmd & PCI_COMMAND_INVALIDATE) {
		cmd |= PCI_COMMAND_INVALIDATE;
		pci_config_writew(pdev, PCI_COMMAND, cmd);
	}
    return -ENOERR;
}
EXPORT_SYMBOL(pci_mwi_enable);

uint32_t pcix_mmrbc_get(struct pci_device *pdev)
{
	uint16_t cap = 0, cmd;

    if ((cmd = pci_config_readw(pdev, cap + PCI_X_CMD)))
		return -EINVAL;

    return 512 << ((cmd & PCI_X_CMD_MAX_READ) >> 2);
}
EXPORT_SYMBOL(pcix_mmrbc_get);

state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc)
{


    return -ENOERR;
}
EXPORT_SYMBOL(pcix_mmrbc_set);
