/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "pci: " fmt

#include <mm.h>
#include <initcall.h>
#include <driver/pci.h>
#include <printk.h>
#include <asm/io.h>

static LIST_HEAD(host_list);

static const enum pci_speed pcix_bus_speed[] = {
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_66MHz_PCIX,
	PCI_SPEED_100MHz_PCIX,
	PCI_SPEED_133MHz_PCIX,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_66MHz_PCIX_ECC,
	PCI_SPEED_100MHz_PCIX_ECC,
	PCI_SPEED_133MHz_PCIX_ECC,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_66MHz_PCIX_266,
	PCI_SPEED_100MHz_PCIX_266,
	PCI_SPEED_133MHz_PCIX_266,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_66MHz_PCIX_533,
	PCI_SPEED_100MHz_PCIX_533,
	PCI_SPEED_133MHz_PCIX_533
};

static const enum pci_speed pcie_bus_speed[] = {
	PCI_SPEED_UNKNOWN,
	PCIE_SPEED_2_5GT,
	PCIE_SPEED_5_0GT,
	PCIE_SPEED_8_0GT,
	PCIE_SPEED_16_0GT,
	PCIE_SPEED_32_0GT,
	PCIE_SPEED_64_0GT,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
	PCI_SPEED_UNKNOWN,
};

static inline enum pci_speed agp_speed(uint32_t status)
{
    bool agp3 = status & 0x08;
    enum pci_speed speed;

    if (status & 0x04)
        speed = AGP_SPEED_4X;
    else if (status & 0x02)
        speed = AGP_SPEED_2X;
    else if (status & 0x01)
        speed = AGP_SPEED_1X;
    else
        return AGP_SPEED_UNKNOWN;

    if (agp3) {
        speed += 2;
        if (speed > AGP_SPEED_8X)
            return AGP_SPEED_UNKNOWN;
    }

    return speed;
}

static inline enum pci_speed pcix_speed(uint32_t status)
{
    if (status & PCI_X_SSTATUS_533MHZ)
        return PCI_SPEED_133MHz_PCIX_533;

    if (status & PCI_X_SSTATUS_266MHZ)
        return PCI_SPEED_133MHz_PCIX_266;

    if (status & PCI_X_SSTATUS_133MHZ) {
        if ((status & PCI_X_SSTATUS_VERS) == PCI_X_SSTATUS_V2)
            return PCI_SPEED_133MHz_PCIX_ECC;
        else
            return PCI_SPEED_133MHz_PCIX;
    }

    return PCI_SPEED_66MHz_PCIX;
}

static void pci_bus_speed_setup(struct pci_bus *bus)
{
    uint32_t val;
    uint8_t pos;

    /* agp device */
    if ((pos = pci_capability_find(bus->bridge, PCI_CAP_ID_AGP)) ||
        (pos = pci_capability_find(bus->bridge, PCI_CAP_ID_AGP3))) {
        val = pci_config_readl(bus->bridge, pos + PCI_AGP_STATUS);
        bus->max_speed = agp_speed(val);
        val = pci_config_readl(bus->bridge, pos + PCI_AGP_COMMAND);
        bus->cur_speed = agp_speed(val);
    }

    /* pcie device */
    if ((pos = pci_capability_find(bus->bridge, PCI_EXP_LNKCAP))) {
        val = pci_config_readl(bus->bridge, pos + PCI_EXP_LNKCAP);
        bus->max_speed = pcie_bus_speed[val & PCI_EXP_LNKCAP_SLS];
    }

    /* pcix device */
    if ((pos = pci_capability_find(bus->bridge, PCI_CAP_ID_PCIX))) {
        val = pci_config_readl(bus->bridge, pos + PCI_X_BRIDGE_SSTATUS);
        bus->max_speed = pcix_speed(val);
        bus->cur_speed = pcix_bus_speed[(val & PCI_X_SSTATUS_FREQ) >> 6];
    }
}

/**
 * pci_res_size - Conversion pci size
 * @defgroup: pci_resource_set
 */
static __always_inline
enum resource_type pci_res_type(uint32_t bar)
{
    if ((bar & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_IO)
        return RESOURCE_PMIO;
    else if ((bar & PCI_BASE_ADDRESS_MEM_TYPE_MASK) == PCI_BASE_ADDRESS_MEM_TYPE_64)
        return RESOURCE_MMIO64;
    else
        return RESOURCE_MMIO;
}

/**
 * pci_res_size - Conversion pci size
 * @defgroup: pci_resource_set
 * @base: base addr
 */
uint64_t pci_res_size(uint64_t base, uint64_t size, uint64_t mask)
{
    u64 rsize = size & mask;

    if (!rsize)
        return 0;

    rsize = rsize & ~(rsize-1);

    if (base == size && ((base | (rsize - 1)) & mask) != mask)
        return 0;

    return rsize;
}

/**
 * pci_resource_set - Setting one resource through pci bar
 * @defgroup: pci_resource_setup
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

    if (sz == 0xffffffff)
        sz = 0;
    if (l == 0xffffffff)
        l = 0;

    if (type == pci_bar_unknown) {
        res->type = pci_res_type(l);
        if (res->type == RESOURCE_PMIO) {
            l64 = l & PCI_BASE_ADDRESS_IO_MASK;
            sz64 = sz & PCI_BASE_ADDRESS_IO_MASK;
            mask64 = PCI_BASE_ADDRESS_IO_MASK & IO_SPACE_LIMIT;
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

    if (res->type == RESOURCE_MMIO64) {
        l = pci_config_readl(pdev, reg + 4);
        pci_config_writel(pdev, reg + 4, l | mask);
        sz = pci_config_readl(pdev, reg + 4);
        pci_config_writel(pdev, reg + 4, l);

        l64 |= ((uint64_t)l << 32);
        sz64 |= ((uint64_t)sz << 32);
        mask64 |= ((uint64_t)~0 << 32);
    }

    if (!sz64)
        goto error;

    sz64 = pci_res_size(l64, sz64, mask64);
    if(!sz64)
        goto error;

    res->start = l64;
    res->size = sz64;

    goto exit;
error:
    res->type = RESOURCE_NONE;
exit:
    return (res->type == RESOURCE_MMIO64) ? 1 : 0;
}

/**
 * pci_resource_setup - Setting pci device resource through pci bar
 * @defgroup: pci_device_setup
 * @pdev: pci device to set
 * @nr: bar number
 */
static void pci_resource_setup(struct pci_device *pdev, int nr, int rom)
{
    struct resource *res;
    int pos, reg;

    for (pos = 0; pos < nr; ++pos) {
        res = &pdev->resource[pos];
        reg = PCI_BASE_ADDRESS_0 + (pos << 2);
        pos += pci_resource_set(pdev, pci_bar_unknown, res, reg);
    }

    if (rom) {
        res = &pdev->resource[PCI_ROM_RESOURCE];
        pci_resource_set(pdev, pci_bar_mem32, res, rom);
    }
}

static inline void pci_bridge_windows_setup(struct pci_device *pdev)
{
    uint32_t val, tmp;

    val = pci_config_readw(pdev, PCI_IO_BASE);
    if (!val) {
        pci_config_writew(pdev, PCI_IO_BASE, 0xe0f0);
        val = pci_config_readw(pdev, PCI_IO_BASE);
        pci_config_writew(pdev, PCI_IO_BASE, 0);
    } if (val) {
        pdev->io_windows = true;
    }

    val = pci_config_readl(pdev, PCI_PREF_MEMORY_BASE);
    if (!val) {
        pci_config_writel(pdev, PCI_PREF_MEMORY_BASE, 0xffe0fff0);
        val = pci_config_readl(pdev, PCI_PREF_MEMORY_BASE);
        pci_config_writel(pdev, PCI_PREF_MEMORY_BASE, 0x0);
    } if (!val) {
        return;
    }

    pdev->pref_window = true;

    if ((val & PCI_PREF_RANGE_TYPE_MASK) == PCI_PREF_RANGE_TYPE_64) {
        val = pci_config_readl(pdev, PCI_PREF_BASE_UPPER32);
        pci_config_writel(pdev, PCI_PREF_BASE_UPPER32, 0xffffffff);
        tmp = pci_config_readl(pdev, PCI_PREF_BASE_UPPER32);
        pci_config_writel(pdev, PCI_PREF_BASE_UPPER32, val);
        if (tmp)
            pdev->pref64_window = true;
    }
}

static inline void pci_subsystem_setup(struct pci_device *pdev)
{
    uint16_t val;

    /* Setup subsystem-vendor */
    val = pci_config_readw(pdev, PCI_SUBSYSTEM_VENDOR_ID);
    pdev->subvendor = val;

    /* Setup subsystem-id */
    val = pci_config_readw(pdev, PCI_SUBSYSTEM_ID);
    pdev->subdevice = val;
}

static inline void pci_cb_subsystem_setup(struct pci_device *pdev)
{
    uint16_t val;

    /* Setup subsystem-vendor */
    val = pci_config_readw(pdev, PCI_CB_SUBSYSTEM_VENDOR_ID);
    pdev->subvendor = val;

    /* Setup subsystem-id */
    val = pci_config_readw(pdev, PCI_CB_SUBSYSTEM_ID);
    pdev->subdevice = val;
}

// static inline void pci_bridge_subsystem_setup(struct pci_device *pdev)
// {
//     uint32_t val, pos;

// 		pos = pci_find_capability(dev, PCI_CAP_ID_SSVID);
//     /* Setup subsystem-vendor */
//     val = pci_config_readw(pdev, PCI_CB_SUBSYSTEM_VENDOR_ID);
//     pdev->subvendor = val;

//     /* Setup subsystem-id */
//     val = pci_config_readw(pdev, PCI_CB_SUBSYSTEM_ID);
//     pdev->subdevice = val;
// }

static inline void pci_irq_setup(struct pci_device *pdev)
{
    uint8_t val;

    val = pci_config_readb(pdev, PCI_INTERRUPT_PIN);
    pdev->pin = val;

    val = pci_config_readb(pdev, PCI_INTERRUPT_LINE);
    pdev->irq = val;
}

/**
 * pci_device_setup - Setup pci device information
 * @defgroup: pci_scan_add_device
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
    if (pdev->head == PCI_HEADER_TYPE_NORMAL){
        if (pdev->class == PCI_CLASS_BRIDGE_PCI)
            goto eclass;
        pci_irq_setup(pdev);
        pci_resource_setup(pdev, 6, PCI_ROM_ADDRESS);
        pci_subsystem_setup(pdev);
    } else if (pdev->head == PCI_HEADER_TYPE_CARDBUS) {
        if (pdev->class != PCI_CLASS_BRIDGE_CARDBUS)
            goto eclass;
        pci_irq_setup(pdev);
        pci_resource_setup(pdev, 1, 0);
        pci_cb_subsystem_setup(pdev);
    } else if (pdev->head == PCI_HEADER_TYPE_BRIDGE) {
        pci_irq_setup(pdev);
        pci_resource_setup(pdev, 2, PCI_ROM_ADDRESS1);
        pci_bridge_windows_setup(pdev);
    } else {
        pr_err("unknown head type %x\n", pdev->head);
        return -EINVAL;
    }

    return -ENOERR;
eclass:
    pci_err(pdev, "bad class %08x, header type %02x\n",
            pdev->class, pdev->head);
    pdev->class = PCI_CLASS_NOT_DEFINED << 8;
    return -ENOERR;
}

/**
 * pci_device_alloc - Allocating for a new device
 * @parent: parent bus
 */
static struct pci_device *pci_device_alloc(struct pci_bus *bus)
{
    struct pci_device *dev;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return NULL;

    dev->bus = bus;
    return dev;
}

/**
 * pci_bus_alloc - Allocating for a new bus
 * @parent: parent bus
 */
static struct pci_bus *pci_bus_alloc(struct pci_bus *parent)
{
    struct pci_bus *bus;

    bus = kzalloc(sizeof(*bus), GFP_KERNEL);
    if (!bus)
        return NULL;

    if (parent) {
        bus->parent = parent;
        bus->ops = parent->ops;
    }

    list_head_init(&bus->child);
    list_head_init(&bus->pci_device_list);
    return bus;
}

static struct pci_bus *pci_bus_add(struct pci_bus *parent, struct pci_device *pdev)
{
    struct pci_bus *child;

    child = pci_bus_alloc(parent);
    if (!child)
        return NULL;

    child->bridge = pdev;
    pci_bus_speed_setup(child);
    list_add(&parent->child, &child->sibling);
    return child;
}

/**
 * pci_scan_device - Scan a device and create a pci device
 */
static state pci_scan_device(struct pci_bus *bus, uint32_t devfn,
                             struct pci_device **sdev)
{
    struct pci_device *dev;
    uint32_t val;
    state retval;

    /* Check whether the device exists */
    val = pci_bus_config_readl(bus, devfn, PCI_VENDOR_ID);
    if (val == 0x00000000 || val == 0xffffffff||
        val == 0x0000ffff || val == 0xffff0000)
        return -ENODEV;

    /* Alloc a pci device */
    dev = pci_device_alloc(bus);
    if (!dev)
        return -ENOMEM;

    dev->devfn = devfn;
    dev->vendor = val & 0xffff;
    dev->device = (val >> 16) & 0xffff;

    retval = pci_device_setup(dev);
    if (retval) {
        kfree(dev);
        return retval;
    }

    *sdev = dev;
    return -ENOERR;
}

static __always_inline
void pci_device_add(struct pci_bus *bus, struct pci_device *dev)
{
    pr_debug("New device: %02d:%02d:%d %06x %04x:%04x\n",
        dev->bus->bus_nr, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn),
        dev->class, dev->vendor, dev->device);

    /* Add pci device to pci bus list */
    list_add(&bus->pci_device_list, &dev->pci_bus_list_pci_device);
    device_register(&dev->dev);
}

/**
 * pci_scan_add_device - Scan a device and add the device to bus
 * @bus: PCI bus to scan
 * @devfn: device number to scan
 */
static state pci_scan_add_device(struct pci_bus *bus, uint32_t devfn)
{
    struct pci_device *dev;
    state ret;

    ret = pci_scan_device(bus, devfn, &dev);
    if (ret)
        return ret;

    pci_device_add(bus, dev);
    return -ENOERR;
}

static uint16_t pci_next_fn(struct pci_bus *bus, uint32_t devfn)
{
    // uint32_t val;
    // val = pci_bus_config_readl(bus, devfn, PCI_ARI_CAP);

    return PCI_FUNC(devfn) + 1;
}

/**
 * pci_scan_slot - Scan a pci slot
 * @bus: PCI bus to scan
 * @slot: slot number to scan
 */
static state pci_scan_slot(struct pci_bus *bus, uint8_t slot)
{
    uint8_t fn;
    state ret;

    /* first check whether slot exists */
    ret = pci_scan_add_device(bus, PCI_DEVFN(slot, 0));
    if (ret)
        return ret;

    for (fn = pci_next_fn(bus, PCI_DEVFN(slot, 0)); fn < 8;
         fn = pci_next_fn(bus, PCI_DEVFN(slot, fn))) {
        ret = pci_scan_add_device(bus, PCI_DEVFN(slot, fn));
    }

    return -ENOERR;
}

static state pci_scan_bridge(struct pci_bus *bus, struct pci_device *pdev)
{
    struct pci_bus *child;
    uint8_t primary, secondary, subordinate;
    uint16_t ctrl;
    uint32_t val;

    val = pci_config_readl(pdev, PCI_PRIMARY_BUS);
    primary = val & 0xff;
    secondary = (val >> 8) & 0xff;
    subordinate = (val >> 16) & 0xff;

    ctrl = pci_config_readw(pdev, PCI_BRIDGE_CONTROL);
    val = ctrl & ~PCI_BRIDGE_CTL_MASTER_ABORT;
    pci_config_writew(pdev, PCI_BRIDGE_CONTROL, ctrl);

    if ((secondary || subordinate) &&
        pdev->head != PCI_HEADER_TYPE_CARDBUS) {
        child = pci_bus_add(bus, pdev);
        child->primary = primary;
        pci_bus_scan(child);
    }

    pci_config_writew(pdev, PCI_BRIDGE_CONTROL, ctrl);
    return -ENOERR;
}

/**
 * pci_bus_scan - Scan all devices of all bus of host
 * @host: pci host
 */
state pci_bus_scan(struct pci_bus *bus)
{
    struct pci_device *pdev;
    uint8_t slot;

    for (slot = 0; slot < 32; ++slot) {
        pci_scan_slot(bus, slot);
    }

    pci_for_each_bridge(pdev, bus) {
        pci_scan_bridge(bus, pdev);
    }

    return -ENOERR;
}

/**
 * pci_host_register - Register PCI bus and probe drive
 * @host:
 */
state pci_host_register(struct pci_host *host)
{
    struct pci_bus *bus;

    /* Alloc host bus */
    bus = pci_bus_alloc(NULL);
    if (!bus)
        return -ENOMEM;

    host->bus = bus;
    bus->ops = host->ops;

    pci_bus_scan(bus);
    pci_bus_devices_probe(bus);

    return -ENOERR;
}
