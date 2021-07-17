/* SPDX-License-Identifier: GPL-2.0-or-later */

#define pr_fmt(fmt) "pci: " fmt

#include <mm.h>
#include <init/initcall.h>
#include <driver/pci.h>
#include "private.h"
#include <printk.h>

#include <asm/io.h>

/* PCI host bridge listhead */
LIST_HEAD(host_list);

static __always_inline
void pci_device_add(struct pci_bus *bus, struct pci_device *dev)
{
    /* Add pci device to pci bus list */
    list_add(&bus->pci_device_list, &dev->pci_bus_list_pci_device);
}

/**
 * pci_res_size - Conversion pci size
 * @defgroup: pci_resource_set
 * 
 */
static __always_inline
enum res_type pci_res_type(uint32_t bar)
{
    if((bar & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
        return RESOURCE_PMIO;
    else if((bar & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64)
        return RESOURCE_MMIO64;
    else
        return RESOURCE_MMIO;
}

/**
 * pci_res_size - Conversion pci size
 * @defgroup: pci_resource_set
 * @base: base addr
 * 
 */
uint64_t pci_res_size(uint64_t base, uint64_t size, uint64_t mask)
{
    u64 rsize = size & mask;

    if(!rsize)
        return 0;

    rsize = rsize & ~(rsize-1);

    if (base == size && ((base | (rsize - 1)) & mask) != mask)
        return 0;

    return rsize;
}

/**
 * pci_resource_set - Setting one resource through pci bar
 * @defgroup: pci_resource_setup
 * 
 */
static int pci_resource_set(struct pci_device *pdev, enum pci_bar_type type,
                    struct resource *res, unsigned int reg)
{
    uint32_t l, sz, mask;
    uint64_t l64, sz64, mask64;
    
    mask = type ? PCI_ROM_ADDRESS_MASK : 0xffffffff;

    l = pci_config_readl(pdev, reg);
    pci_config_writel(pdev, reg, l | mask);
    sz = pci_config_readl(pdev, reg);
    pci_config_writel(pdev, reg, l);

    if(sz == 0xffffffff)
        sz = 0;
    if(l == 0xffffffff)
        l = 0;

    if(type == pci_bar_unknown) {
        res->type = pci_res_type(l);
        if(res->type == RESOURCE_PMIO) {
            l64 = l & PCI_BASE_ADDRESS_IO_MASK;
            sz64 = sz & PCI_BASE_ADDRESS_IO_MASK;
            mask64 = PCI_BASE_ADDRESS_IO_MASK & (u32)IO_SPACE_LIMIT;
        } else {
            l64 = l & PCI_BASE_ADDRESS_MEM_MASK;
            sz64 = sz & PCI_BASE_ADDRESS_MEM_MASK;
            mask64 = (uint32_t)PCI_BASE_ADDRESS_MEM_MASK;
        }
    } else {
        if (l & PCI_ROM_ADDRESS_ENABLE);
            // res->flags |= IORESOURCE_ROM_ENABLE;
        l64 = l & PCI_ROM_ADDRESS_MASK;
        sz64 = sz & PCI_ROM_ADDRESS_MASK;
        mask64 = PCI_ROM_ADDRESS_MASK;
    }

    if(res->type == RESOURCE_MMIO64) {
        l = pci_config_readl(pdev, reg + 4);
        pci_config_writel(pdev, reg + 4, l | mask);
        sz = pci_config_readl(pdev, reg + 4);
        pci_config_writel(pdev, reg + 4, l);

        l64 |= ((uint64_t)l << 32);
        sz64 |= ((uint64_t)sz << 32);
        mask64 |= ((uint64_t)~0 << 32);
    }

    if(!sz64)
        goto error;

    sz64 = pci_res_size(l64, sz64, mask64);
    if(!sz64)
        goto error;
    
    res->start = l64;
    res->end = l64 + sz64 - 1;

    goto exit;

error:
    res->type = RESOURCE_NONE;
exit:
    return (res->type & RESOURCE_MMIO64) ? 1 : 0;
}

/**
 * pci_resource_setup - Setting pci device resource through pci bar
 * @defgroup: pci_device_setup
 * @pdev: pci device to set
 * @nr: bar number
 * 
 */
static void pci_resource_setup(struct pci_device *pdev, int nr, int rom)
{
    struct resource *res;
    int pos, reg;

    for(pos = 0; pos < nr; ++pos) {
        res = &pdev->resource[pos];
        reg = PCI_BASE_ADDRESS_0 + (pos << 2);
        pos += pci_resource_set(pdev, pci_bar_unknown, res, reg);
    }

    if(rom) {
        res = &pdev->resource[PCI_ROM_RESOURCE];
        pci_resource_set(pdev, pci_bar_mem32, res, rom);
    }
}

/**
 * pci_device_setup - Setup pci device information
 * @defgroup: pci_scan_add_device
 * 
 */
static state pci_device_setup(struct pci_device *pdev)
{
    uint32_t val;

    pdev->dev.bus = &pci_bus_type;

    /* Setup revision and class code */
    val = pci_config_readl(pdev, PCI_CLASS_REVISION);
    pdev->revision = val & 0xff;
    pdev->class = val >> 8;

    /* Setup Header type */
    val = pci_config_readb(pdev, PCI_HEADER_TYPE);
    pdev->head = val & PCI_HEADER_TYPE_MASK;
    pdev->multifunction = !!(val & 0x80);

    /* Setup resource form bar */
    if(pdev->head == PCI_HEADER_TYPE_NORMAL){
        pci_resource_setup(pdev, 6, PCI_ROM_ADDRESS);

        /* Setup subsystem-vendor */
        val = pci_config_readw(pdev, PCI_SUBSYSTEM_VENDOR_ID);
        pdev->subvendor = val;

        /* Setup subsystem-id */
        val = pci_config_readw(pdev, PCI_SUBSYSTEM_ID);
        pdev->subdevice = val;

    } else if(pdev->head == PCI_HEADER_TYPE_BRIDGE) {
        pci_resource_setup(pdev, 2, PCI_ROM_ADDRESS1);
    }

    return -ENOERR;
}

/**
 * pci_bus_alloc - Allocating for a new bus
 * @parent: parent bus
 * 
 */
static struct pci_bus *pci_bus_alloc(struct pci_bus *parent)
{
    struct pci_bus *bus;

    bus = kzalloc(sizeof(*bus), GFP_KERNEL);
    if(!bus)
        return NULL;

    list_head_init(&bus->children);
    list_head_init(&bus->node);
    list_head_init(&bus->pci_device_list);

    bus->parent = parent;

    return bus;
}

/**
 * pci_bus_alloc - Allocating for a new bus
 * @parent: parent bus
 * 
 */
static struct pci_device *pci_device_alloc(struct pci_bus *bus)
{
    struct pci_device *dev;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if(!dev)
        return NULL;
    
    dev->bus = bus;

    return dev;
}

/**
 * pci_scan_device - Scan a device and create a pci device
 * 
 */
static state pci_scan_device(struct pci_bus *bus, uint32_t devfn, 
                             struct pci_device **sdev)
{
    struct pci_device *dev;
    uint32_t val;

    /* Check whether the device exists */
    val = pci_bus_config_readl(bus, devfn, PCI_VENDOR_ID);
    if( val == 0x00000000 || val == 0xffffffff ||
        val == 0x0000ffff || val == 0xffff0000 ) 
        return -ENODEV;

    /* Alloc a pci device */
    dev = pci_device_alloc(bus);
    if(!dev)
        return -ENOMEM;

    dev->devfn = devfn;
    dev->vendor = val & 0xffff;
    dev->device = (val >> 16) & 0xffff;

    if(pci_device_setup(dev)) {
        kfree(dev);
    }

    *sdev = dev;
    return -ENOERR;
}

/**
 * pci_scan_add_device - Scan a device and add the device to bus
 * @bus: PCI bus to scan
 * @devfn: device number to scan
 * 
 */
static state pci_scan_add_device(struct pci_bus *bus, uint32_t devfn)
{
    struct pci_device *dev;
    state ret;

    ret = pci_scan_device(bus, devfn, &dev);
    if(ret)
        return ret;

    pci_device_add(bus, dev);

    pr_info("New device: %02d:%02d:%d %04x %04x:%04x\n\r",
        dev->bus->bus_nr, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn),
        dev->class, dev->vendor, dev->device);

    return -ENOERR;
}

static uint16_t fn_next(struct pci_bus *bus, uint32_t devfn)
{
    // uint32_t val;


    // val = pci_bus_config_readl(bus, devfn, PCI_ARI_CAP);

    
    return PCI_FUNC(devfn) + 1;
}


/**
 * pci_scan_slot - Scan a slot
 * @bus: PCI bus to scan
 * @slot: slot number to scan
 * 
 */
static state pci_scan_slot(struct pci_bus *bus, uint8_t slot)
{
    uint8_t fn = 0;
    state ret;

    /* First check whether slot exists */   
    ret = pci_scan_add_device(bus, PCI_DEVFN(slot, fn));
    if(ret)
        return ret;

    for(fn = fn_next(bus, PCI_DEVFN(slot, 0)); fn < 8; 
        fn = fn_next(bus, PCI_DEVFN(slot, fn))) 
    {
        ret = pci_scan_add_device(bus, PCI_DEVFN(slot, fn));
    }
    return -ENOERR;
}

/**
 * pci_scan_host - Scan all devices of all bus of host :)
 * @host: pci host
 * 
 */
state pci_scan_bus(struct pci_bus *bus)
{
    uint8_t slot;

    for(slot = 0; slot < 32; ++slot) {
        pci_scan_slot(bus, slot);
    }

    return -ENOERR;
}

/**
 * pci_host_register - Register PCI bus and probe drive
 * 
 * @param host 
 * @return state 
 */
state pci_host_register(struct pci_host *host)
{
    struct pci_bus *bus;

    /* Alloc host bus */
    host->bus = pci_bus_alloc(NULL);
    if(!host->bus)
        return -ENOMEM;

    bus = host->bus;
    bus->ops = host->ops;

    list_add_prev(&host_list, &bus->node);

    return -ENOERR;
}

state pci_host_probe(void)
{
    struct pci_bus *bus;

    list_for_each_entry(bus, &host_list, node) {
        /* Scan all devices on pci root bus */
        pci_scan_bus(bus);

        /* Start all devices on pci root bus */
        pci_bus_devices_probe(bus);
    }
    return -ENOERR;
}

/* Synchronize all PCI drivers */
driver_initcall_sync(pci_host_probe);
