/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVTABLE_H_
#define _DEVTABLE_H_

#include <types.h>
#include <bits.h>

#define ACPI_ID_LEN	9

struct acpi_device_id {
    char id[ACPI_ID_LEN];
    void *data;
};

#define DT_NAME_LEN 32
#define DT_TYPE_LEN 32
#define DT_COMPATIBLE_LEN 32

struct dt_device_id {
    char name[DT_NAME_LEN];
    char type[DT_TYPE_LEN];
    char compatible[DT_COMPATIBLE_LEN];
    const void *data;
};

#define PLATFORM_NAME_LEN 32

struct platform_device_id {
    char name[PLATFORM_NAME_LEN];
    const void *data;
};

#define DMI_NAME_LEN 79

enum dmi_region_type {
    DMI_REG_BIOS_VENDOR,
    DMI_REG_BIOS_VERSION,
    DMI_REG_BIOS_DATE,
    DMI_REG_BIOS_RELEASE,
    DMI_REG_EC_RELEASE,
    DMI_REG_SYS_VENDOR,
	DMI_REG_PRODUCT_NAME,
	DMI_REG_PRODUCT_VERSION,
	DMI_REG_PRODUCT_SERIAL,
	DMI_REG_PRODUCT_UUID,
	DMI_REG_PRODUCT_SKU,
	DMI_REG_PRODUCT_FAMILY,
	DMI_REG_BOARD_VENDOR,
	DMI_REG_BOARD_NAME,
	DMI_REG_BOARD_VERSION,
	DMI_REG_BOARD_SERIAL,
	DMI_REG_BOARD_ASSET_TAG,
	DMI_REG_CHASSIS_VENDOR,
	DMI_REG_CHASSIS_TYPE,
	DMI_REG_CHASSIS_VERSION,
	DMI_REG_CHASSIS_SERIAL,
	DMI_REG_CHASSIS_ASSET_TAG,
    DMI_REG_MAX,
};

struct dmi_reg_match {
    enum dmi_region_type reg;
    char substr[DMI_NAME_LEN];
    bool exact_match;
};

struct dmi_device_id {
    char name[DMI_NAME_LEN];
    struct dmi_reg_match matches[4];
    const void *data;
};

#define DMI_MATCH(Reg, Str) {       \
    .reg = (Reg), .substr = (Str),  \
}

#define DMI_MATCH_EXACT(Reg, Str) { \
    .reg = (Reg), .substr = (Str),  \
    .exact_match = true,            \
}

#define SDIO_ANY_ID  (~0)

struct sdio_device_id {
    uint16_t vendor;
    uint16_t device;
    uint8_t class;
};

#define SDIO_DEVICE(Vendor, Device) \
    .class = SDIO_ANY_ID, \
    .vendor = (Vendor), .device = (Device)

#define SDIO_DEVICE_CLASS(class) \
    .class = class, \
    .vendor = SDIO_ANY_ID, .device = SDIO_ANY_ID

#define PCI_ANY_ID  (~0)

struct pci_device_id {
    uint32_t vendor,    device;
    uint32_t subvendor, subdevice;
    uint32_t class,     class_mask;
    const void *data;
};

#define PCI_DEVICE(Vendor, Device) \
    .vendor = (Vendor), .device = (Device), \
    .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID

#define PCI_DEVICE_CLASS(Class, ClassMask) \
    .class = (Class), .class_mask = (ClassMask),   \
    .vendor = PCI_ANY_ID, .device = PCI_ANY_ID, \
    .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID

enum usb_match_mode {
    USB_DEV_MATCH_VENDOR        = 0x0001,
    USB_DEV_MATCH_PRODUCT       = 0x0002,

    USB_DEV_MATCH_DEV_LO        = 0x0004,
    USB_DEV_MATCH_DEV_HI        = 0x0008,
    USB_DEV_MATCH_DEV_CLASS     = 0x0010,
    USB_DEV_MATCH_DEV_SUBCLASS  = 0x0020,
    USB_DEV_MATCH_DEV_PROTOCOL  = 0x0040,

    USB_DEV_MATCH_INT_CLASS     = 0x0080,
    USB_DEV_MATCH_INT_SUBCLASS  = 0x0100,
    USB_DEV_MATCH_INT_PROTOCOL  = 0x0200,
    USB_DEV_MATCH_INT_NUMBER    = 0x0400,
};

struct usb_device_id {
    uint16_t idVendor;
    uint16_t idProduct;

    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint16_t bcdDevice_lo;
    uint16_t bcdDevice_hi;

    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t bInterfaceNumber;

    const void *data;
    enum usb_match_mode match_mode;
};

#define USB_DEVICE(Vendor, Product) \
    .match_mode = USB_DEV_MATCH_VENDOR | USB_DEV_MATCH_PRODUCT, \
    .idVendor = (Vendor), .idProduct = (Product)

#define USB_DEVICE_CLASS(Class) \
    .match_mode = USB_DEV_MATCH_DEV_CLASS, .bDeviceClass = (Class)

#define USB_INT_CLASS(Class) \
    .match_mode = USB_DEV_MATCH_INT_CLASS, .bInterfaceClass = (Class)

#define SERIO_ANY_ID  (~0)

struct serio_device_id {
    uint8_t type;
    uint8_t extra;
    uint8_t id;
    uint8_t proto;
    const void *data;
};

#define SERIO_DEVICE(Type) \
    .type = (Type), .extra = SERIO_ANY_ID, \
    .id = SERIO_ANY_ID, .proto = SERIO_ANY_ID

#define SERIO_DEVICE_PROTO(Type, Proto) \
    .type = (Type), .extra = SERIO_ANY_ID \
    .id = SERIO_ANY_ID, .proto = (Proto)

#endif /* _DEVTABLE_H_ */
