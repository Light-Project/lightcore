/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/pci.h>
#include <asm/io.h>
#include <export.h>

void __weak *pci_ioport_map(struct pci_device *pdev, resource_size_t port, size_t len)
{
    return ioport_map(port, len);
}
EXPORT_SYMBOL(pci_ioport_map);

void *pci_resource_ioremap_range(struct pci_device *pdev, unsigned int bar, size_t offset, size_t limit)
{
    resource_size_t addr = pci_resource_start(pdev, bar);
    resource_size_t size = pci_resource_size(pdev, bar);
    enum resource_type type = pci_resource_type(pdev, bar);

    if (!addr || size <= offset)
        return NULL;

    addr += offset;
    size -= offset;

    if (limit && limit < size)
        size = limit;

    if (type == RESOURCE_PMIO)
        return pci_ioport_map(pdev, addr, size);
    else if (type == RESOURCE_MMIO)
        return pci_ioremap(pdev, addr, size);

    return NULL;
}
EXPORT_SYMBOL(pci_resource_ioremap_range);

void *pci_resource_ioremap_range_wc(struct pci_device *pdev, unsigned int bar, size_t offset, size_t limit)
{
    resource_size_t addr = pci_resource_start(pdev, bar);
    resource_size_t size = pci_resource_size(pdev, bar);
    enum resource_type type = pci_resource_type(pdev, bar);

    if (type != RESOURCE_MMIO)
        return NULL;

    if (!addr || size <= offset)
        return NULL;

    addr += offset;
    size -= offset;

    if (limit && limit < size)
        size = limit;

    return pci_ioremap(pdev, addr, size);
}
EXPORT_SYMBOL(pci_resource_ioremap_range_wc);

void *pci_resource_ioremap(struct pci_device *pdev, unsigned int bar, size_t limit)
{
    return pci_resource_ioremap_range(pdev, bar, 0, limit);
}
EXPORT_SYMBOL(pci_resource_ioremap);

void *pci_resource_ioremap_wc(struct pci_device *pdev, unsigned int bar, size_t limit)
{
    return pci_resource_ioremap_range_wc(pdev, bar, 0, limit);
}
EXPORT_SYMBOL(pci_resource_ioremap_wc);

void pci_resource_iounmap(struct pci_device *pdev, void *block)
{

}
EXPORT_SYMBOL(pci_resource_iounmap);
