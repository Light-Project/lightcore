/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PCI_H_
#define _DRIVER_PCI_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <resource.h>
#include <lightcore/pci.h>
#include <devtable.h>

#include <driver/pci/pci_ids.h>
#include <driver/pci/pci_regs.h>

extern struct bus_type pci_bus_type;

enum pci_resource {
    /* standard PCI resources */
    PCI_STD_RESOURCES,
    PCI_STD_RESOURCE_END = PCI_STD_RESOURCES + PCI_STD_NUM_BARS - 1,

    /* expansion ROM resource */
    PCI_ROM_RESOURCE,

	/* resources assigned to buses behind the bridge */
    PCI_BRIDGE_RESOURCES,

    PCI_RESOURCE_MAX,
};

enum pci_bar_type {
    pci_bar_unknown,    /* Standard PCI BAR probe */
    pci_bar_io,         /* An I/O port BAR */
    pci_bar_mem32,      /* A 32-bit memory BAR */
    pci_bar_mem64,      /* A 64-bit memory BAR */
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

struct pci_device {
    struct device dev;              /* Generic device */
    list_t pci_bus_list_pci_device; /* Node of pci_bus->pci_device_list */
    struct pci_bus *bridge;         /* Bus this device bridges to */

    /* Device characteristic */
    struct pci_bus *bus;
    uint16_t devfn;         /* Pci device and function number */
    uint16_t vendor;        /* 0x00: Vendor identification */
    uint16_t device;        /* 0x02: Device identification */
    uint8_t  revision;      /* 0x08: Revision identification */
    uint32_t class;         /* 0x09: Class code */
    uint8_t  head;          /* 0x0e: Header type */
    uint16_t subvendor;     /* 0x2c: Subsystem vendor identification */
    uint16_t subdevice;     /* 0x2e: Subsystem identification */
    uint8_t  irq;           /* 0x3c: Interrupt line */
    uint8_t  pin;           /* 0x3d: Interrupt pin */

    /* Pci device flags */
    uint32_t io_windows:1;      /* Bridge has io window */
    uint32_t pref_window:1;     /* Bridge has pref window */
    uint32_t pref64_window:1;   /* Bridge has pref64 window */
    uint32_t multifunction:1;   /* Multi-function device */

    uint32_t flags_added:1;     /* Device has been added */

    /* I/O resource */
    struct resource resource[PCI_RESOURCE_MAX];
};

#define device_to_pci_device(dpt) \
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
#define pci_resource_type(pdev, bar)    ((pdev)->resource[(bar)].type)

struct pci_driver {
    struct driver driver;
    struct list_head list;
    const struct pci_device_id *id_table;

    /* Operation function */
    state (*probe)(struct pci_device *pdev, void *pdata);
    void (*remove)(struct pci_device *pdev);
    void (*shutdown)(struct pci_device *pdev);
    state (*suspend)(struct pci_device *pdev, pm_message_t state);
    state (*resume)(struct pci_device *pdev);
};

#define driver_to_pci_driver(dpt) \
        (container_of((dpt), struct pci_driver, driver))

struct pci_bus {
    struct device dev;

    struct pci_device *bridge;  /* Bridge device of this bus */
    struct pci_bus *parent;     /* Parent bus on this bridge */
    struct list_head child;     /* List head of child buses */
    struct list_head sibling;   /* Node of children list */
    unsigned char bus_nr;
    unsigned char primary;

    enum pci_speed max_speed;
    enum pci_speed cur_speed;

    struct list_head pci_device_list;
    struct pci_ops *ops;
};

#define pci_for_each_device(pci_device, pci_bus) \
    list_for_each_entry(pci_device, &pci_bus->pci_device_list, pci_bus_list_pci_device)

#define pci_for_each_bridge(pci_bridge, pci_bus)        \
    pci_for_each_device(pci_bridge, pci_bus)            \
    if (pci_bridge->head == PCI_HEADER_TYPE_BRIDGE ||   \
        pci_bridge->head == PCI_HEADER_TYPE_CARDBUS)

struct pci_host {
    struct device   dev;
    struct pci_ops  *ops;
    struct pci_bus  *bus;   /* Root bus */
    void *priv;             /* Private data */
};

struct pci_ops {
    state (*read)(struct pci_bus *bus, uint devfn, uint reg,int size, uint32_t *val);
    state (*write)(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t val);
};

/* base.c - PCI basic operation */
uint8_t  pci_bus_config_readb(struct pci_bus *bus, unsigned int devfn, unsigned int reg);
uint16_t pci_bus_config_readw(struct pci_bus *bus, unsigned int devfn, unsigned int reg);
uint32_t pci_bus_config_readl(struct pci_bus *bus, unsigned int devfn, unsigned int reg);
void pci_bus_config_writeb(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint8_t val);
void pci_bus_config_writew(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint16_t val);
void pci_bus_config_writel(struct pci_bus *bus, unsigned int devfn, unsigned int reg, uint32_t val);

uint8_t  pci_config_readb(struct pci_device *pdev, unsigned int reg);
uint16_t pci_config_readw(struct pci_device *pdev, unsigned int reg);
uint32_t pci_config_readl(struct pci_device *pdev, unsigned int reg);
void pci_config_writeb(struct pci_device *pdev, unsigned int reg, uint8_t val);
void pci_config_writew(struct pci_device *pdev, unsigned int reg, uint16_t val);
void pci_config_writel(struct pci_device *pdev, unsigned int reg, uint32_t val);

uint8_t pci_capability_find(struct pci_device *pdev, int cap);
state pci_cacheline_size(struct pci_device *pdev);
state pci_mwi_enable(struct pci_device *pdev);
void pci_mwi_disable(struct pci_device *pdev);
uint32_t pcix_mmrbc_get(struct pci_device *pdev);
state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc);


/* raw.c - PCI Raw operation */
state pci_raw_config_read(unsigned int domain, unsigned int bus, unsigned int devfn, unsigned int reg, int size, uint32_t *val);
state pci_raw_config_write(unsigned int domain, unsigned int bus, unsigned int devfn, unsigned int reg, int size, uint32_t val);

/* find.c */
struct pci_bus *pci_find_bus(unsigned int domain, unsigned int bus);

/* bus.c */
void pci_bus_devices_probe(const struct pci_bus *bus);

/* driver.c */
state pci_driver_register(struct pci_driver *);
void pci_driver_unregiste(struct pci_driver *drv);

/* core.c */
state pci_bus_scan(struct pci_bus *bus);
state pci_host_register(struct pci_host *host);
state pci_host_unregister(struct pci_host *host);

#define pci_emerg(pdev, fmt, ...)   dev_emerg(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_alert(pdev, fmt, ...)   dev_alert(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_crit(pdev, fmt, ...)    dev_crit(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_err(pdev, fmt, ...)     dev_err(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_warn(pdev, fmt, ...)    dev_warn(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_notice(pdev, fmt, ...)  dev_notice(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_info(pdev, fmt, ...)    dev_info(&(pdev)->dev, fmt, ##__VA_ARGS__)
#define pci_dbg(pdev, fmt, ...)     dev_debug(&(pdev)->dev, fmt, ##__VA_ARGS__)

#endif /* _DRIVER_PCI_H_ */
