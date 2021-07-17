/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PCI_H_
#define _DRIVER_PCI_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <resource.h>
#include <lightcore/pci.h>
#include <mod_devicetable.h>

#include <driver/pci/pci_ids.h>
#include <driver/pci/pci_regs.h>

extern struct bus_type pci_bus_type;

enum pci_resource {
    /* #0-5: standard PCI resources */
    PCI_STD_RESOURCES,
    PCI_STD_RESOURCE_END = PCI_STD_RESOURCES + PCI_STD_NUM_BARS - 1,

    /* #6: expansion ROM resource */
    PCI_ROM_RESOURCE,

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
    AGP_UNKNOWN                 = 0x0c,
    AGP_1X                      = 0x0d,
    AGP_2X                      = 0x0e,
    AGP_4X                      = 0x0f,
    AGP_8X                      = 0x10,
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

    /* Bus structure description */
    list_t pci_bus_list_pci_device; /* Node of pci_bus->pci_device_list */
    struct pci_bus *bridge;         /* Bus this device bridges to */
    struct device dev;              /* Generic device */

    /* Device characteristic */
    struct pci_bus *bus;
    uint16_t devfn;         /* Pci device and function number */
    uint16_t vendor;        /* 0x00: Vendor identification */
    uint16_t device;        /* 0x02: Device identification */
    uint8_t  revision;      /* 0x08: Revision identification */
    uint16_t class;         /* 0x09: Class code */
    uint8_t  head;          /* 0x0e: Header type */
    uint16_t subvendor;     /* 0x2c: Subsystem vendor identification */
    uint16_t subdevice;     /* 0x2e: Subsystem identification */
    uint8_t  irq;           /* 0x3c: Interrupt line */
    uint8_t  pin;           /* 0x3d: Interrupt pin */

    /* Pci device flags */
    uint32_t multifunction:1;   /* multi-function device */
    uint32_t flags_added:1;     /*  */

    /* I/O resource */
    struct resource resource[PCI_RESOURCE_MAX];

};

#define device_to_pci_device(dpt) \
        (container_of((dpt), struct pci_device, dev))

static inline void *pci_get_devdata(const struct pci_device *pdev)
{
    return dev_get_devdata(&pdev->dev);
}
static inline void pci_set_devdata(struct pci_device *pdev, void *data)
{
    dev_set_devdata(&pdev->dev, data);
}

#define pci_resource_start(pdev, bar)   ((pdev)->resource[(bar)].start)
#define pci_resource_end(pdev, bar)     ((pdev)->resource[(bar)].end)
#define pci_resource_type(pdev, bar)    ((pdev)->resource[(bar)].type)
#define pci_resource_size(pdev, bar)            \
        (pci_resource_end((pdev), (bar)) -      \
        pci_resource_start((pdev), (bar)) + 1)

struct pci_driver {
    list_t  list;
    struct driver driver;
    const struct pci_device_id *id_table;

    /* Operation function */
    state (*probe)(struct pci_device *pdev, int id);
    state (*remove)(struct pci_device *pdev);
    void (*shutdown)(struct pci_device *pdev);
    state (*suspend)(struct pci_device *pdev, pm_message_t state);
    state (*resume)(struct pci_device *pdev);
};

#define driver_to_pci_driver(dpt) \
        (container_of((dpt), struct pci_driver, driver))

state pci_driver_register(struct pci_driver *);
void pci_driver_unregiste(struct pci_driver *drv);

/* pci_bus structure declare the PCI bus */
struct pci_bus {

    /* Bus structure description */
    list_t  node;               /* Node of children list */
    list_t  children;           /* List head of child buses */
    struct pci_bus *parent;     /* Parent bus on this bridge */
    
    struct list_head pci_device_list;   /* List head of pci_devices on this bus */
    struct pci_ops *ops;   

    uint    bus_nr;
};

#define pci_bus_for_each_pci_device(pci_device, pci_bus)    \
        list_for_each_entry(pci_device, &pci_bus->pci_device_list, pci_bus_list_pci_device)

/* The pci_ops structure declare the operation method of PCI */
struct pci_ops {
    state (*read)(struct pci_bus *bus, uint devfn, uint reg,int size, uint32_t *val);
    state (*write)(struct pci_bus *bus, uint devfn, uint reg, int size, uint32_t val);
};

/* The pci_host structure declare PCI controller */
struct pci_host {
    struct device   dev;
    struct pci_ops  *ops;   
    struct pci_bus  *bus;   /* Root bus */
    void *priv;             /* Private data */
};

state pci_host_register(struct pci_host *host);

/* pci/base.c - PCI basic operation */
uint8_t pci_bus_config_readb(struct pci_bus *bus, uint devfn, uint reg);
uint16_t pci_bus_config_readw(struct pci_bus *bus, uint devfn, uint reg);
uint32_t pci_bus_config_readl(struct pci_bus *bus, uint devfn, uint reg);
void pci_bus_config_writeb(struct pci_bus *bus, uint devfn, uint reg, uint8_t val);
void pci_bus_config_writew(struct pci_bus *bus, uint devfn, uint reg, uint16_t val);
void pci_bus_config_writel(struct pci_bus *bus, uint devfn, uint reg, uint32_t val);

uint8_t pci_config_readb(struct pci_device *pdev, uint reg);
uint16_t pci_config_readw(struct pci_device *pdev, uint reg);
uint32_t pci_config_readl(struct pci_device *pdev, uint reg);
void pci_config_writeb(struct pci_device *pdev, uint reg, uint8_t val);
void pci_config_writew(struct pci_device *pdev, uint reg, uint16_t val);
void pci_config_writel(struct pci_device *pdev, uint reg, uint32_t val);

state pci_cacheline_size(struct pci_device *pdev);

void pci_mwi_clear(struct pci_device *pdev);
state pci_mwi_set(struct pci_device *pdev);

uint32_t pcix_mmrbc_get(struct pci_device *pdev);
state pcix_mmrbc_set(struct pci_device *pdev, uint32_t mmrbc);

#endif /* _DRIVER_PCI_H_ */
