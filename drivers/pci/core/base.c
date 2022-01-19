/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <export.h>

#define PCI_BUS_CONFIG_READ_OP(name, len, type)             \
state pci_bus_config_##name(struct pci_bus *bus,            \
    unsigned int devfn, unsigned int reg, type *val)        \
{                                                           \
    uint32_t data;                                          \
    state ret;                                              \
    ret = bus->ops->read(bus, devfn, reg, len, &data);      \
    *val = (type)data;                                      \
    return ret;                                             \
}

#define PCI_BUS_CONFIG_WRITE_OP(name, len, type)            \
state pci_bus_config_##name(struct pci_bus *bus,            \
    unsigned int devfn, unsigned int reg, type val)         \
{                                                           \
    state ret;                                              \
    ret = bus->ops->write(bus, devfn, reg, len, val);       \
    return ret;                                             \
}

PCI_BUS_CONFIG_READ_OP(readb, 1, uint8_t)
PCI_BUS_CONFIG_READ_OP(readw, 2, uint16_t)
PCI_BUS_CONFIG_READ_OP(readl, 4, uint32_t)
PCI_BUS_CONFIG_WRITE_OP(writeb, 1, uint8_t)
PCI_BUS_CONFIG_WRITE_OP(writew, 2, uint16_t)
PCI_BUS_CONFIG_WRITE_OP(writel, 4, uint32_t)

EXPORT_SYMBOL(pci_bus_config_readb);
EXPORT_SYMBOL(pci_bus_config_readw);
EXPORT_SYMBOL(pci_bus_config_readl);
EXPORT_SYMBOL(pci_bus_config_writeb);
EXPORT_SYMBOL(pci_bus_config_writew);
EXPORT_SYMBOL(pci_bus_config_writel);

#define PCI_CONFIG_TRY_READ_OP(name, type)                  \
state pci_config_try_##name(struct pci_device *pdev,        \
                        unsigned int reg, type *val)        \
{                                                           \
    return pci_bus_config_##name                            \
                (pdev->bus, pdev->devfn, reg, val);         \
}

#define PCI_CONFIG_TRY_WRITE_OP(name, type)                 \
state pci_config_try_##name(struct pci_device *pdev,        \
                        unsigned int reg, type val)         \
{                                                           \
    return pci_bus_config_##name                            \
                (pdev->bus, pdev->devfn, reg, val);         \
}

PCI_CONFIG_TRY_READ_OP(readb, uint8_t)
PCI_CONFIG_TRY_READ_OP(readw, uint16_t)
PCI_CONFIG_TRY_READ_OP(readl, uint32_t)
PCI_CONFIG_TRY_WRITE_OP(writeb, uint8_t)
PCI_CONFIG_TRY_WRITE_OP(writew, uint16_t)
PCI_CONFIG_TRY_WRITE_OP(writel, uint32_t)

EXPORT_SYMBOL(pci_config_try_readb);
EXPORT_SYMBOL(pci_config_try_readw);
EXPORT_SYMBOL(pci_config_try_readl);
EXPORT_SYMBOL(pci_config_try_writeb);
EXPORT_SYMBOL(pci_config_try_writew);
EXPORT_SYMBOL(pci_config_try_writel);

#define PCI_CONFIG_READ_OP(name, type)                  \
type pci_config_##name(struct pci_device *pdev,         \
                       unsigned int reg)                \
{                                                       \
    type val;                                           \
    pci_config_try_##name(pdev, reg, &val);             \
    return val;                                         \
}

#define PCI_CONFIG_WRITE_OP(name, type)                 \
void pci_config_##name(struct pci_device *pdev,         \
                       unsigned int reg, type val)      \
{                                                       \
    pci_config_try_##name(pdev, reg, val);              \
}

PCI_CONFIG_READ_OP(readb, uint8_t)
PCI_CONFIG_READ_OP(readw, uint16_t)
PCI_CONFIG_READ_OP(readl, uint32_t)
PCI_CONFIG_WRITE_OP(writeb, uint8_t)
PCI_CONFIG_WRITE_OP(writew, uint16_t)
PCI_CONFIG_WRITE_OP(writel, uint32_t)

EXPORT_SYMBOL(pci_config_readb);
EXPORT_SYMBOL(pci_config_readw);
EXPORT_SYMBOL(pci_config_readl);
EXPORT_SYMBOL(pci_config_writeb);
EXPORT_SYMBOL(pci_config_writew);
EXPORT_SYMBOL(pci_config_writel);

static inline uint8_t
pci_capability_find_next_ttl(struct pci_device *pdev, uint8_t pos, int cap, int ttl)
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

static inline uint8_t
pci_capability_find_next(struct pci_device *pdev, uint8_t pos, int cap)
{
    return pci_capability_find_next_ttl(pdev, pos, cap, PCI_CAP_FIND_TTL);
}

static inline uint8_t
pci_capability_find_start(struct pci_device *pdev, uint8_t head)
{
    uint16_t val;

    val = pci_config_readw(pdev, PCI_STATUS);
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

    return pci_capability_find_next(pdev, pos, cap);
}
EXPORT_SYMBOL(pci_capability_find);

state pci_cacheline_size(struct pci_device *pdev)
{
    return -ENOERR;
}
EXPORT_SYMBOL(pci_cacheline_size);

/**
 * pci_mwi_clear - memory-write-invalidate PCI transaction disables
 * @pdev: device to disable
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

/**
 * pci_mwi_enable - memory-write-invalidate PCI transaction enables
 * @pdev: device to enable
 */
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

/**
 * pcix_mmrbc_get - set PCI-X maximum memory read byte count
 *
 */
uint32_t pcix_mmrbc_get(struct pci_device *pdev)
{
    uint16_t cap = 0, cmd;

    if ((cmd = pci_config_readw(pdev, cap + PCI_X_CMD)))
        return -EINVAL;

    return 512 << ((cmd & PCI_X_CMD_MAX_READ) >> 2);
}
EXPORT_SYMBOL(pcix_mmrbc_get);

/**
 * pcix_mmrbc_set - set PCI-X maximum memory read byte count
 *
 */
state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc)
{
    return -ENOERR;
}
EXPORT_SYMBOL(pcix_mmrbc_set);

static void pci_set_master(struct pci_device *pdev, bool enable)
{
    uint16_t val;

    val = pci_config_readw(pdev, PCI_COMMAND);
    if (!(val & PCI_COMMAND_MASTER) == enable)
        return;

    pci_debug(pdev, "bus mastering %s\n",
        enable ? "enable" : "disable");

    if (enable)
        val |= PCI_COMMAND_MASTER;
    else
        val &= ~PCI_COMMAND_MASTER;

    pci_config_writew(pdev, PCI_COMMAND, val);
    pdev->busmaster = enable;
}

void pci_master_enable(struct pci_device *pdev)
{
    pci_set_master(pdev, true);
    pcibios_master_set(pdev);
}
EXPORT_SYMBOL(pci_master_enable);

void pci_master_disable(struct pci_device *pdev)
{
    pci_set_master(pdev, false);
}
EXPORT_SYMBOL(pci_master_disable);
