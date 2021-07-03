#include <export.h>
#include <driver/pci.h>


uint8_t pci_bus_config_readb(struct pci_bus *bus, 
                            uint devfn, uint reg)
{
    uint32_t val;
    bus->ops->read(bus, devfn, reg, 1, &val);
    return val;
}
EXPORT_SYMBOL(pci_bus_config_readb);

uint16_t pci_bus_config_readw(struct pci_bus *bus, 
                            uint devfn, uint reg)
{
    uint32_t val;
    bus->ops->read(bus, devfn, reg, 2, &val);
    return val;
}
EXPORT_SYMBOL(pci_bus_config_readw);

uint32_t pci_bus_config_readl(struct pci_bus *bus, 
                            uint devfn, uint reg)
{
    uint32_t val;
    bus->ops->read(bus, devfn, reg, 4, &val);
    return val;
}
EXPORT_SYMBOL(pci_bus_config_readl);

void pci_bus_config_writeb(struct pci_bus *bus, 
                            uint devfn, uint reg, uint8_t val)
{
    bus->ops->write(bus, devfn, reg, 1, val);
}
EXPORT_SYMBOL(pci_bus_config_writeb);

void pci_bus_config_writew(struct pci_bus *bus, 
                            uint devfn, uint reg, uint16_t val)
{
    bus->ops->write(bus, devfn, reg, 2, val);
}
EXPORT_SYMBOL(pci_bus_config_writew);

void pci_bus_config_writel(struct pci_bus *bus, 
                            uint devfn, uint reg, uint32_t val)
{
    bus->ops->write(bus, devfn, reg, 4, val);
}
EXPORT_SYMBOL(pci_bus_config_writel);

uint8_t pci_config_readb(struct pci_device *pdev, uint reg)
{
    return pci_bus_config_readb(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readb);

uint16_t pci_config_readw(struct pci_device *pdev, uint reg)
{
    return pci_bus_config_readw(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readw);

uint32_t pci_config_readl(struct pci_device *pdev, uint reg)
{
    return pci_bus_config_readl(pdev->bus, pdev->devfn, reg);
}
EXPORT_SYMBOL(pci_config_readl);

void pci_config_writeb(struct pci_device *pdev, uint reg, uint8_t val)
{
    pci_bus_config_writeb(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writeb);

void pci_config_writew(struct pci_device *pdev, uint reg, uint16_t val)
{
    pci_bus_config_writew(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writew);

void pci_config_writel(struct pci_device *pdev, uint reg, uint32_t val)
{
    pci_bus_config_writel(pdev->bus, pdev->devfn, reg, val);
}
EXPORT_SYMBOL(pci_config_writel);

state pci_cacheline_size(struct pci_device *pdev)
{
	// uint8_t cacheline;

    // cacheline = pci_config_readb(pdev, PCI_CACHE_LINE_SIZE);

    return -ENOERR;
}
EXPORT_SYMBOL(pci_cacheline_size);

/**
 * pci_mwi_clear - disables Memory-Write-Invalidate for device dev
 * @dev: the PCI device to disable
 *
 * Disables PCI Memory-Write-Invalidate transaction on the device
 */
void pci_mwi_clear(struct pci_device *pdev)
{
	uint16_t cmd;

	cmd = pci_config_readw(pdev, PCI_COMMAND);
	if (cmd & PCI_COMMAND_INVALIDATE) {
		cmd &= ~PCI_COMMAND_INVALIDATE;
		pci_config_writew(pdev, PCI_COMMAND, cmd);
	}
}
EXPORT_SYMBOL(pci_mwi_clear);

state pci_mwi_set(struct pci_device *pdev)
{
	uint16_t cmd;

	cmd = pci_config_readw(pdev, PCI_COMMAND);
	if (cmd & PCI_COMMAND_INVALIDATE) {
		cmd |= PCI_COMMAND_INVALIDATE;
		pci_config_writew(pdev, PCI_COMMAND, cmd);
	}
    return -ENOERR;
}
EXPORT_SYMBOL(pci_mwi_set);

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
