
#include <init/initcall.h>
#include <driver/pci.h>
#include <printk.h>


static struct pci_device_id piix4_pci_tbl[] = {
    {
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_82371AB_3,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_82443MX_3,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_IXP200_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_IXP300_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_IXP400_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SBX00_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_AMD,
        .device = PCI_DEVICE_ID_AMD_HUDSON2_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_AMD,
        .device = PCI_DEVICE_ID_AMD_KERNCZ_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_HYGON,
        .device = PCI_DEVICE_ID_AMD_KERNCZ_SMBUS,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_SERVERWORKS,
        .device = PCI_DEVICE_ID_SERVERWORKS_OSB4,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_SERVERWORKS,
        .device = PCI_DEVICE_ID_SERVERWORKS_CSB5,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_SERVERWORKS,
        .device = PCI_DEVICE_ID_SERVERWORKS_CSB6,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_SERVERWORKS,
        .device = PCI_DEVICE_ID_SERVERWORKS_HT1000SB,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    {
        .vendor = PCI_VENDOR_ID_SERVERWORKS,
        .device = PCI_DEVICE_ID_SERVERWORKS_HT1100LD,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
    },
    { }, /* NULL */
};

static state piix4_probe(struct pci_device *pdev, int pdata)
{


    return -ENOERR;
}

static state piix4_remove(struct pci_device *pdev)
{


    return -ENOERR;
}

static struct pci_driver piix4_driver = {
    .driver = {
        .name = "i2c-piix4"
    },
    .id_table = piix4_pci_tbl,
    .probe = piix4_probe,
    .remove = piix4_remove,
};

static state piix4_init(void)
{
    return pci_driver_register(&piix4_driver);
}

driver_initcall(piix4_init);
