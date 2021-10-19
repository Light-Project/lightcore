/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

#define PCI_CAP_FIND_TTL 48

uint8_t pci_bus_config_readb(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 1, &val);

    return val;
}

uint16_t pci_bus_config_readw(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 2, &val);

    return val;
}

uint32_t pci_bus_config_readl(struct pci_bus *bus,
            unsigned int devfn, unsigned int reg)
{
    uint32_t val;

    bus->ops->read(bus, devfn, reg, 4, &val);

    return val;
}

void pci_bus_config_writeb(struct pci_bus *bus, unsigned int devfn,
                                    unsigned int reg, uint8_t val)
{
    bus->ops->write(bus, devfn, reg, 1, val);
}

void pci_bus_config_writew(struct pci_bus *bus, unsigned int devfn,
                                    unsigned int reg, uint16_t val)
{
    bus->ops->write(bus, devfn, reg, 2, val);
}

void pci_bus_config_writel(struct pci_bus *bus, unsigned int devfn,
                           unsigned int reg, uint32_t val)
{
    bus->ops->write(bus, devfn, reg, 4, val);
}

uint8_t pci_config_readb(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readb(pdev->bus, pdev->devfn, reg);
}

uint16_t pci_config_readw(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readw(pdev->bus, pdev->devfn, reg);
}

uint32_t pci_config_readl(struct pci_device *pdev, unsigned int reg)
{
    return pci_bus_config_readl(pdev->bus, pdev->devfn, reg);
}

void pci_config_writeb(struct pci_device *pdev, unsigned int reg, uint8_t val)
{
    pci_bus_config_writeb(pdev->bus, pdev->devfn, reg, val);
}

void pci_config_writew(struct pci_device *pdev, unsigned int reg, uint16_t val)
{
    pci_bus_config_writew(pdev->bus, pdev->devfn, reg, val);
}

void pci_config_writel(struct pci_device *pdev, unsigned int reg, uint32_t val)
{
    pci_bus_config_writel(pdev->bus, pdev->devfn, reg, val);
}

static uint8_t pci_capability_find_next_ttl(struct pci_device *pdev, uint8_t pos,
                                            int cap, int ttl)
{
    uint16_t ent;
    uint8_t id;

    pos = pci_config_readb(pdev, pos);

    while (ttl--) {
        if (pos < 0x40)
            break;

        pos &= ~3;
        ent = pci_config_readw(pdev, pos);

        id = ent & 0xff;
        if (id == 0xff)
            break;
        if (id == cap)
            return pos;

        pos = ent >> 8;
    }

    return 0;
}

static uint8_t pci_capability_find_start(struct pci_device *pdev, uint8_t head)
{
    uint16_t val = pci_config_readw(pdev, PCI_STATUS);

    if (!(val & PCI_STATUS_CAP_LIST))
        return 0;

    switch (head) {
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

    pos = pci_capability_find_start(pdev, pdev->head);
    if (!pos)
        return 0;

    return pci_capability_find_next_ttl(pdev, pos, cap, PCI_CAP_FIND_TTL);
}

state pci_cacheline_size(struct pci_device *pdev)
{
    return -ENOERR;
}

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

uint32_t pcix_mmrbc_get(struct pci_device *pdev)
{
    uint16_t cap = 0, cmd;

    if ((cmd = pci_config_readw(pdev, cap + PCI_X_CMD)))
        return -EINVAL;

    return 512 << ((cmd & PCI_X_CMD_MAX_READ) >> 2);
}

state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc)
{
    return -ENOERR;
}

bool pci_power_no_d1d2(struct pci_device *pdev)
{
    bool parent_state = false;

    if (pdev->bus->bridge)
        parent_state = pdev->bus->bridge->power_no_d1d2;
    return pdev->power_no_d1d2 || parent_state;
}

state pci_device_power_set(struct pci_device *pdev, enum pci_power power)
{

    if (power < PCI_POWER_D0 || power > PCI_POWER_D3C)
        return -EINVAL;



    return -ENOERR;
}

state pci_power_on(struct pci_device *pdev)
{
    return pci_device_power_set(pdev, PCI_POWER_D0);
}

state pci_power_set(struct pci_device *pdev, enum pci_power power)
{
    power = clamp(power, PCI_POWER_D0, PCI_POWER_D3C);
    if ((power == PCI_POWER_D1 || power == PCI_POWER_D2) && pci_power_no_d1d2(pdev))
        return -ENOERR;

    if (pdev->current_state == power)
        return -ENOERR;

    return -ENOERR;
}

state pci_device_enable(struct pci_device *pdev, int bar)
{

    return -ENOERR;
}

state pci_enable(struct pci_device *pdev)
{

    return -ENOERR;
}

EXPORT_SYMBOL(pci_bus_config_readb);
EXPORT_SYMBOL(pci_bus_config_readw);
EXPORT_SYMBOL(pci_bus_config_readl);
EXPORT_SYMBOL(pci_bus_config_writeb);
EXPORT_SYMBOL(pci_bus_config_writew);
EXPORT_SYMBOL(pci_bus_config_writel);
EXPORT_SYMBOL(pci_config_readb);
EXPORT_SYMBOL(pci_config_readw);
EXPORT_SYMBOL(pci_config_readl);
EXPORT_SYMBOL(pci_config_writeb);
EXPORT_SYMBOL(pci_config_writew);
EXPORT_SYMBOL(pci_config_writel);

EXPORT_SYMBOL(pci_capability_find);
EXPORT_SYMBOL(pci_cacheline_size);
EXPORT_SYMBOL(pci_mwi_disable);
EXPORT_SYMBOL(pci_mwi_enable);
EXPORT_SYMBOL(pcix_mmrbc_get);
EXPORT_SYMBOL(pcix_mmrbc_set);
