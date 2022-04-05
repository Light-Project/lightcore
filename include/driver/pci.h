/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PCI_H_
#define _DRIVER_PCI_H_

#include <device.h>
#include <resource.h>
#include <devtable.h>
#include <printk.h>
#include <driver/pci/pci_ids.h>
#include <driver/pci/pci_regs.h>
#include <lightcore/pci.h>

extern struct bus_type pci_bus_type;

#define PCI_MAX_FUNCTION    8
#define PCI_MAX_SLOT        32
#define PCI_CAP_FIND_TTL    48

#define PCI_PM_D2_WAIT      200 /* usec */
#define PCI_PM_D3H_WAIT     10  /* msec */
#define PCI_PM_D3C_WAIT     100 /* msec */

/* Total number of bridge resources for P2P and CardBus */
#define PCI_BRIDGE_RESOURCE_NUM 4

/* PCI-to-PCI (P2P) bridge windows */
#define PCI_BRIDGE_IO_WINDOW        (PCI_BRIDGE_RESOURCES + 0)
#define PCI_BRIDGE_MEM_WINDOW       (PCI_BRIDGE_RESOURCES + 1)
#define PCI_BRIDGE_PREF_MEM_WINDOW  (PCI_BRIDGE_RESOURCES + 2)

/* CardBus bridge windows */
#define PCI_CB_BRIDGE_IO_0_WINDOW   (PCI_BRIDGE_RESOURCES + 0)
#define PCI_CB_BRIDGE_IO_1_WINDOW   (PCI_BRIDGE_RESOURCES + 1)
#define PCI_CB_BRIDGE_MEM_0_WINDOW  (PCI_BRIDGE_RESOURCES + 2)
#define PCI_CB_BRIDGE_MEM_1_WINDOW  (PCI_BRIDGE_RESOURCES + 3)

#define PCI_COMMAND_DECODE_ENABLE   (PCI_COMMAND_MEMORY | PCI_COMMAND_IO)

enum pci_resource {
    /* Standard PCI resources */
    PCI_STD_RESOURCES,
    PCI_STD_RESOURCE_END = PCI_STD_RESOURCES + PCI_STD_NUM_BARS - 1,

    /* Expansion ROM resource */
    PCI_ROM_RESOURCE,

    /* Resources assigned to buses behind the bridge */
    PCI_BRIDGE_RESOURCES,
    PCI_BRIDGE_RESOURCES_END = PCI_BRIDGE_RESOURCES + PCI_BRIDGE_RESOURCE_NUM - 1,

    PCI_RESOURCE_MAX,
};

enum pci_bar {
    PCI_BAR_UNKNOWN,    /* Standard PCI BAR probe */
    PCI_BAR_IO,         /* An I/O port BAR */
    PCI_BAR_MEM32,      /* A 32-bit memory BAR */
    PCI_BAR_MEM64,      /* A 64-bit memory BAR */
};

enum pci_speed {
    PCI_SPEED_33MHz             = 0x00,
    PCI_SPEED_66MHz             = 0x01,
    PCI_SPEED_66MHz_PCIX        = 0x02,
    PCI_SPEED_100MHz_PCIX       = 0x03,
    PCI_SPEED_133MHz_PCIX		= 0x04,
    PCI_SPEED_66MHz_PCIX_ECC	= 0x05,
    PCI_SPEED_100MHz_PCIX_ECC	= 0x06,
    PCI_SPEED_133MHz_PCIX_ECC	= 0x07,
    PCI_SPEED_66MHz_PCIX_266	= 0x09,
    PCI_SPEED_100MHz_PCIX_266	= 0x0a,
    PCI_SPEED_133MHz_PCIX_266	= 0x0b,

    AGP_SPEED_UNKNOWN           = 0x0c,
    AGP_SPEED_1X                = 0x0d,
    AGP_SPEED_2X                = 0x0e,
    AGP_SPEED_4X                = 0x0f,
    AGP_SPEED_8X                = 0x10,

    PCI_SPEED_66MHz_PCIX_533    = 0x11,
    PCI_SPEED_100MHz_PCIX_533   = 0x12,
    PCI_SPEED_133MHz_PCIX_533   = 0x13,

    PCIE_SPEED_2_5GT            = 0x14,
    PCIE_SPEED_5_0GT            = 0x15,
    PCIE_SPEED_8_0GT            = 0x16,
    PCIE_SPEED_16_0GT           = 0x17,
    PCIE_SPEED_32_0GT           = 0x18,
    PCIE_SPEED_64_0GT           = 0x19,

    PCI_SPEED_UNKNOWN           = 0xff,
};

enum pci_power {
    PCI_POWER_D0                = 0x00,
    PCI_POWER_D1                = 0x01,
    PCI_POWER_D2                = 0x02,
    PCI_POWER_D3H               = 0x03,
    PCI_POWER_D3C               = 0x04,
    PCI_POWER_ERR               = 0x05,
    PCI_POWER_UNKNOWN           = 0x06,
};

struct pci_device {
    struct list_head list;      /* Node of pci_bus->pci_device_list */
    struct pci_bus *bridge;     /* Bus this device bridges to */
    struct device dev;          /* Generic device */

    /* Device characteristic */
    struct pci_bus *bus;
    unsigned int devfn;         /* PCI device and function number */
    unsigned int cfgsz;         /* PCI config space size */
    uint16_t vendor;            /* 0x00: Vendor identification */
    uint16_t device;            /* 0x02: Device identification */
    uint8_t  revision;          /* 0x08: Revision identification */
    uint32_t class;             /* 0x09: Class code */
    uint8_t  head;              /* 0x0e: Header type */
    uint16_t subvendor;         /* 0x2c: Subsystem vendor identification */
    uint16_t subdevice;         /* 0x2e: Subsystem identification */
    uint8_t  irq;               /* 0x3c: Interrupt line */
    uint8_t  pin;               /* 0x3d: Interrupt pin */

    uint8_t  devcap;            /* PCIE Device Capabilities */
    uint8_t  pcie_cap;          /* PCIE capability offset */
    uint8_t  msi_cap;           /* MSI capability offset */
    uint8_t  msix_cap;          /* MSI-X capability offset */
    uint8_t  pm_cap;            /* MSI-X capability offset */

    uint16_t pcie_flags;        /* Cached PCIe Capabilities Register */
    uint8_t  pcie_mpss;         /* PCIe Max Payload Size Supported */

    unsigned int d3h_delay;     /* D3H to D0 transition time */
    unsigned int d3c_delay;     /* D3C to D0 transition time */

    /* PCI device power */
    enum pci_power power_state; /* Current operating state */
    uint32_t power_has_d1:1;    /* Low power state D1 is supported */
    uint32_t power_has_d2:1;    /* Low power state D2 is supported */
    uint32_t power_no_d1d2:1;   /* D1 and D2 are forbidden */
    uint32_t power_no_d3cold:1; /* D3cold is forbidden */
    uint32_t pme_support:5;     /* PME# support */
    uint32_t pme_poll:1;        /* Poll device's PME status bit  */

    /* PCI device flags */
    uint32_t io_windows:1;      /* Bridge has io window */
    uint32_t mmio_always:1;     /* Disallow turning off io/mem */
	uint32_t msi_enabled:1;     /* MSI is enabled */
	uint32_t msix_enabled:1;    /* MSIX is enabled */
    uint32_t pref_window:1;     /* Bridge has pref window */
    uint32_t pref64_window:1;   /* Bridge has pref64 window */
    uint32_t multifunction:1;   /* Multi-function device */
    uint32_t busmaster:1;       /* Device is busmaster */
    uint32_t flags_added:1;     /* Device has been added */

    /* I/O resource */
    struct resource resource[PCI_RESOURCE_MAX];
};

#define device_to_pci(dpt) \
    container_of(dpt, struct pci_device, dev)

static inline void *pci_get_devdata(const struct pci_device *pdev)
{
    return device_get_pdata(&pdev->dev);
}

static inline void pci_set_devdata(struct pci_device *pdev, void *data)
{
    device_set_pdata(&pdev->dev, data);
}

#define pci_resource_start(pdev, bar)   ((pdev)->resource[(bar)].start)
#define pci_resource_size(pdev, bar)    ((pdev)->resource[(bar)].size)
#define pci_resource_type(pdev, bar)    (resource_type(&pdev->resource[bar]))

struct pci_driver {
    struct driver driver;
    struct list_head list;
    const struct pci_device_id *id_table;

    /* Operation function */
    state (*probe)(struct pci_device *pdev, const void *pdata);
    void (*remove)(struct pci_device *pdev);
    void (*shutdown)(struct pci_device *pdev);
    state (*suspend)(struct pci_device *pdev, pm_message_t state);
    state (*resume)(struct pci_device *pdev);
};

#define driver_to_pci(dpt) \
    container_of((dpt), struct pci_driver, driver)

struct pci_bus {
    struct list_head sibling;   /* Node of children list */
    struct list_head child;     /* List head of children buses */
    struct pci_host *host;      /* Parent bus on this bridge */
    struct pci_bus *parent;     /* Parent bus on this bridge */
    struct pci_device *bridge;  /* Bridge device of this bus */

    unsigned char bus_nr;
    unsigned char primary;
    enum pci_speed max_speed;
    enum pci_speed cur_speed;

    struct device dev;
    struct list_head pci_device_list;
    struct pci_ops *ops;
};

#define pci_for_each_device(pci_device, pci_bus) \
    list_for_each_entry(pci_device, &pci_bus->pci_device_list, list)

#define pci_for_each_bridge(pci_bridge, pci_bus) \
    pci_for_each_device(pci_bridge, pci_bus) \
        if (pci_is_bridge(pci_bridge))

struct pci_host {
    struct list_head list;
    struct device *parent;
    struct pci_bus *root;
    struct pci_ops *ops;
    void *priv;
};

struct pci_ops {
    state (*read)(struct pci_bus *bus, unsigned int devfn, unsigned int reg, int size, uint32_t *val);
    state (*write)(struct pci_bus *bus, unsigned int devfn, unsigned int reg, int size, uint32_t val);
    uint8_t (*irq_swizzle)(struct pci_device *pdev, uint8_t *pin);
    unsigned int (*irq_map)(struct pci_device *pdev, uint8_t slot, uint8_t pin);
};

/**
 * pci_is_root_bus -
 * @pdev: PCI device to check
 */
static inline bool pci_is_root_bus(struct pci_bus *pbus)
{
    return !(pbus->parent);
}

/**
 * pci_is_bridge -
 * @pdev: PCI device to check
 */
static inline bool pci_is_bridge(struct pci_device *pdev)
{
    return pdev->head == PCI_HEADER_TYPE_BRIDGE ||
           pdev->head == PCI_HEADER_TYPE_CARDBUS;
}

/**
 * pci_upstream_bridge -
 * @pdev: PCI device to get
 */
static inline struct pci_device *pci_upstream_bridge(struct pci_device *pdev)
{
    if (pci_is_root_bus(pdev->bus))
        return NULL;

    return pdev->bus->bridge;
}

/**
 * pci_is_pcie - check if the PCI device is PCI Express capable
 * @pdev: PCI device to check
 */
static inline bool pci_is_pcie(struct pci_device *pdev)
{
    return !!pdev->pcie_cap;
}

/**
 * pci_pcie_type - get the PCIE device/port type
 * @pdev: PCI device to get
 */
static inline uint8_t pci_pcie_type(struct pci_device *pdev)
{
    return (pdev->pcie_cap & PCI_EXP_FLAGS_TYPE) >> 4;
}

/**
 * pci_pcie_is_downstream -
 * @pdev: PCI device to get
 */
static inline bool pci_pcie_is_downstream(struct pci_device *pdev)
{
    uint8_t type = pci_pcie_type(pdev);

    return type == PCI_EXP_TYPE_ROOT_PORT  ||
           type == PCI_EXP_TYPE_DOWNSTREAM ||
           type == PCI_EXP_TYPE_PCIE_BRIDGE;
}

/**
 * pci_no_d1d2 -
 * @pdev: PCI device to get
 */
static inline bool pci_no_d1d2(struct pci_device *pdev)
{
    bool pstate = false;

    if (pdev->bus->bridge)
        pstate = pdev->bus->bridge->power_no_d1d2;

    return pdev->power_no_d1d2 || pstate;
}

/* PCI iomap helper */
extern void __malloc *pci_resource_ioremap_range(struct pci_device *pdev, unsigned int bar, size_t offset, size_t limit);
extern void __malloc *pci_resource_ioremap_range_wc(struct pci_device *pdev, unsigned int bar, size_t offset, size_t limit);
extern void __malloc *pci_resource_ioremap(struct pci_device *pdev, unsigned int bar, size_t limit);
extern void __malloc *pci_resource_ioremap_wc(struct pci_device *pdev, unsigned int bar, size_t limit);
extern void pci_resource_iounmap(struct pci_device *pdev, void *block);

/* PCI basic operation */
extern state pci_bus_config_readb(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint8_t *val);
extern state pci_bus_config_readw(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint16_t *val);
extern state pci_bus_config_readl(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint32_t *val);
extern state pci_bus_config_writeb(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint8_t val);
extern state pci_bus_config_writew(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint16_t val);
extern state pci_bus_config_writel(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint32_t val);

extern state pci_config_try_readb(struct pci_device *pdev, unsigned int reg, uint8_t *val);
extern state pci_config_try_readw(struct pci_device *pdev, unsigned int reg, uint16_t *val);
extern state pci_config_try_readl(struct pci_device *pdev, unsigned int reg, uint32_t *val);
extern state pci_config_try_writeb(struct pci_device *pdev, unsigned int reg, uint8_t val);
extern state pci_config_try_writew(struct pci_device *pdev, unsigned int reg, uint16_t val);
extern state pci_config_try_writel(struct pci_device *pdev, unsigned int reg, uint32_t val);

extern uint8_t pci_config_readb(struct pci_device *pdev, unsigned int reg);
extern uint16_t pci_config_readw(struct pci_device *pdev, unsigned int reg);
extern uint32_t pci_config_readl(struct pci_device *pdev, unsigned int reg);
extern void pci_config_writeb(struct pci_device *pdev, unsigned int reg, uint8_t val);
extern void pci_config_writew(struct pci_device *pdev, unsigned int reg, uint16_t val);
extern void pci_config_writel(struct pci_device *pdev, unsigned int reg, uint32_t val);

extern uint8_t pci_capability_find(struct pci_device *pdev, int cap);
extern state pci_cacheline_size(struct pci_device *pdev);
extern state pci_device_enable_type(struct pci_device *pdev, enum resource_type type);
extern state pci_device_enable_mmio(struct pci_device *pdev);
extern state pci_device_enable_pmio(struct pci_device *pdev);
extern state pci_device_enable(struct pci_device *pdev);
extern state pci_mwi_enable(struct pci_device *pdev);
extern void pci_mwi_disable(struct pci_device *pdev);
extern uint32_t pcix_mmrbc_get(struct pci_device *pdev);
extern state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc);
extern void pci_master_enable(struct pci_device *pdev);
extern void pci_master_disable(struct pci_device *pdev);

/* PCI architecture specific */
extern void pcibios_master_set(struct pci_device *pdev);
extern void pcibios_device_add(struct pci_device *pdev);
extern void pcibios_device_remove(struct pci_device *pdev);
extern state pcibios_device_enable(struct pci_device *pdev, uint16_t bars);
extern void pcibios_device_disable(struct pci_device *pdev, uint16_t bars);
extern void pcibios_bus_add(struct pci_bus *pbus);
extern void pcibios_bus_remove(struct pci_bus *pbus);
extern state pcibios_irq_alloc(struct pci_device *pdev);
extern void pcibios_irq_free(struct pci_device *pdev);

/* PCI helper functions */
extern state pci_raw_config_read(unsigned int domain, unsigned int bus, unsigned int devfn, unsigned int reg, int size, uint32_t *val);
extern state pci_raw_config_write(unsigned int domain, unsigned int bus, unsigned int devfn, unsigned int reg, int size, uint32_t val);
extern struct pci_device *pci_get_device(struct pci_bus *pbus, unsigned int devfn);
extern struct pci_bus *pci_find_bus(unsigned int domain, unsigned int bus);
extern struct pci_device *pci_find_device(const struct pci_device_id *id, struct pci_device *form);
extern struct pci_bus *pci_bus_find_ancestor(const struct pci_bus *bus);
extern struct pci_host *pci_bus_find_host(const struct pci_bus *bus);

/* PCI resource */
extern state pci_resource_enable(struct pci_device *pdev, uint16_t bars);

/* PCI bus driver */
extern void pci_bus_devices_probe(const struct pci_bus *bus);
extern state pci_driver_register(struct pci_driver *);
extern void pci_driver_unregiste(struct pci_driver *);
extern void pci_irq_setup(struct pci_device *);

/* PCI power management */
extern void pci_power_update_state(struct pci_device *pdev, enum pci_power power);
extern state pci_power_on(struct pci_device *pdev);
extern state pci_power_state_set(struct pci_device *pdev, enum pci_power power);
extern void pci_power_setup(struct pci_device *pdev);

/* PCI core */
extern bool pci_resource_set(struct pci_device *pdev, enum pci_bar type, struct resource *res, unsigned int reg);
extern state pci_bus_scan(struct pci_bus *bus);
extern state pci_host_register(struct pci_host *host);
extern state pci_host_unregister(struct pci_host *host);

#define pci_emerg(pdev, fmt, ...)   dev_emerg(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_alert(pdev, fmt, ...)   dev_alert(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_crit(pdev, fmt, ...)    dev_crit(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_err(pdev, fmt, ...)     dev_err(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_warn(pdev, fmt, ...)    dev_warn(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_notice(pdev, fmt, ...)  dev_notice(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_info(pdev, fmt, ...)    dev_info(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_debug(pdev, fmt, ...)   dev_debug(&(pdev)->dev, fmt, ##__VA_ARGS__)
BUS_DEVRES_GENERIC(pci, struct pci_device, dev)

#endif /* _DRIVER_PCI_H_ */
