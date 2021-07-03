#include <driver/pci.h>
#include <asm/io.h>
#include <mm.h>
#include <init/initcall.h>

static inline uint32_t pci_read(uint bus, uint devfn, uint8_t reg, int size)
{
    uint32_t addr = (1<<31)|(bus<<16)|(PCI_SLOT(devfn)<<11)|(PCI_FUNC(devfn)<<8)|reg;
    outl(0x0cf8, addr);
    switch(size) {
        case 1:
            return inb(0x0cfc);
        case 2:
            return inw(0x0cfc);
        default:
            return inl(0x0cfc);
    }
    return 0;
}

static inline void pci_write(uint bus, uint devfn, uint8_t reg, int size, uint32_t val)
{
    uint32_t addr = (1<<31)|(bus<<16)|(PCI_SLOT(devfn)<<11)|(PCI_FUNC(devfn)<<8)|reg;
    outl(0x0cf8, addr);
    switch(size) {
        case 1:
            outb(0x0cfc, val);
        case 2:
            outw(0x0cfc, val);
        default:
            outl(0x0cfc, val);
    }
}

static state pci_config_read(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t *val)
{
    *val = pci_read(bus->bus_nr, devfn, reg, size);
    return -ENOERR;
}

static state pci_config_write(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t val)
{
    pci_write(bus->bus_nr, devfn, reg, size, val);
    return -ENOERR;
}

static struct pci_ops i386_pci_ops = {
    .read = pci_config_read,
    .write = pci_config_write,
};

static state i386_pci_init(void)
{
    struct pci_host *host;

    host = kmalloc(sizeof(*host), GFP_KERNEL);
    if(!host)
        return -ENOMEM;

    host->ops = &i386_pci_ops;

    return pci_host_register(host);
}

driver_initcall_sync(i386_pci_init);
