/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_BASE_H_
#define _DEVICE_BASE_H_

#include <types.h>
#include <state.h>
#include <stddef.h>		// for container_of
#include <klist.h>

#include <system/mutex.h>
#include <system/kobject.h>
#include <system/pm.h>

typedef struct bus_type bus_type_t;
typedef struct driver driver_t;
typedef struct device device_t;

#include <device.h>
#include <device/bus.h>
#include <device/class.h>
#include <device/driver.h>
#include <mod_devicetable.h>

struct device_id{
    union{
        struct acpi_device_id acpi;
        struct dt_device_id dt;
        struct pci_device_id pci;
        struct usb_device_id usb;
    };
};


#endif
