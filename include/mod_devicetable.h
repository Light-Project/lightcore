#ifndef _MOD_DEVTABLE_H_
#define _MOD_DEVTABLE_H_

#define ACPI_ID_LEN	9

struct acpi_device_id {
    char id[ACPI_ID_LEN]; 
};

#define PLATFORM_NAME_LEN 32

struct platform_device_id {
    char        name[PLATFORM_NAME_LEN];
    const void *data;
};

#define DT_NAME_LEN 32

struct dt_device_id {
    char        name[32];
    char        type[32];
    char        compatible[128];
    const void *data;
};


#define PCI_ANY_ID (~0)

struct pci_device_id {
    uint32_t vendor,    device;
    uint32_t subvendor, subdevice;
    uint32_t class,     class_mask;
    int      driver_data;
};

struct usb_device_id {
    uint8_t idVendor;
    uint8_t idProduct;
    uint8_t bcdDevice_lo;
    uint8_t bcdDevice_hi;
};

#endif /* _MOD_DEVTABLE_H_ */
