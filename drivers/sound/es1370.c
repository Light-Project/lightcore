/* SPDX-License-Identifier: GPL-2.0-or-later */

#define DRIVER_NAME "snd-es1370"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <driver/pci.h>
#include <init/initcall.h>
#include <driver/sound/es1370.h>
#include <driver/sound/codec-ak4531.h>

/**
 * es1370_codec_write - ak4531 operation function
 * 
 * @param val 
 */
static void es1370_codec_write(uint16_t reg, uint16_t val)
{
	outw(ES_REG_1370_CODEC, ES_1370_CODEC_WRITE(reg, val));
}

static const struct pci_device_id es1370_pci_tbl[] = {
    {
        .vendor = PCI_VENDOR_ID_ENSONIQ,
        .device = PCI_DEVICE_ID_ENSONIQ_ES1370,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    { }
};

static state es1370_probe(struct pci_device *pdev, int pdata)
{
    
    return -ENOERR;
}

static state es1370_remove(struct pci_device *pdev)
{
    
    return -ENOERR;
}

static struct pci_driver es1370_pci_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = es1370_pci_tbl,
    .probe = es1370_probe,
    .remove = es1370_remove,
};

static state es1370_init(void)
{
    return pci_driver_register(&es1370_pci_driver);
}

driver_initcall(es1370_init);
