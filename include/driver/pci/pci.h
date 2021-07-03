
#include <types.h>

#include "pci_ids.h"

#define PCI_ANY_ID (~0)

/**
 * struct pci_device_id - PCI device ID structure
 * @vendor:		Vendor ID to match (or PCI_ANY_ID)
 * @device:		Device ID to match (or PCI_ANY_ID)
 * @subvendor:		Subsystem vendor ID to match (or PCI_ANY_ID)
 * @subdevice:		Subsystem device ID to match (or PCI_ANY_ID)
 * @class:		Device class, subclass, and "interface" to match.
 *			See Appendix D of the PCI Local Bus Spec or
 *			include/linux/pci_ids.h for a full list of classes.
 *			Most drivers do not need to specify class/class_mask
 *			as vendor/device is normally sufficient.
 * @class_mask:		Limit which sub-fields of the class field are compared.
 *			See drivers/scsi/sym53c8xx_2/ for example of usage.
 * @driver_data:	Data private to the driver.
 *			Most drivers don't need to use driver_data field.
 *			Best practice is to use driver_data as an index
 *			into a static list of equivalent device types,
 *			instead of using it as a pointer.
 */
struct pci_device_id {
    uint32_t vendor, device;		/* Vendor and device ID or PCI_ANY_ID*/
    uint32_t subvendor, subdevice;	/* Subsystem ID's or PCI_ANY_ID */
    uint32_t class, class_mask;	    /* (class,subclass,prog-if) triplet */
    ulong_t  driver_data;	        /* Data private to the driver */
};