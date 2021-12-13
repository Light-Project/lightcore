/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DMI_H_
#define _DRIVER_DMI_H_

#include <types.h>
#include <state.h>
#include <slist.h>
#include <devtable.h>

#define SM_ANCHOR   "_SM_"
#define DMI_ANCHOR  "_DMI_"

struct dmi_entry_point {
    uint8_t anchor[4];      // 0x00: _SM_, specified as four ASCII characters
    uint8_t checksum;       // 0x04: Checksum of the Entry Point Structure
    uint8_t length;         // 0x05: Length of the Entry Point Structure
    uint8_t major;          // 0x06: Major version of this specification
    uint8_t minor;          // 0x07: Minor version of this specification
    uint16_t maxsize;       // 0x08: Size of the largest SMBIOS structure
    uint8_t entryrev;       // 0x0a: EPS revision implemented in this structure and identifies
    uint8_t formatted[5];   // 0x0b: Value present in the Entry Point Revision field
    uint8_t inanchor[5];    // 0x10: _DMI_, specified as five ASCII characters
    uint8_t inchecksum;     // 0x15: Checksum of Intermediate Entry Point Structure
    uint16_t strlen;        // 0x16: Total length of SMBIOS Structure Table
    uint32_t straddr;       // 0x18: Physical address of SMBIOS Structure Table
    uint16_t strnr;         // 0x1c: Total number of structures present in the SMBIOS Structure Table
    uint8_t bcdrev;         // 0x1e: Indicates compliance with a revision of this specification
} __packed;

#define SM3_ANCHOR  "_SM3_"

struct dmi3_entry_point {
    uint8_t anchor[5];      // 0x00: _SM3_, specified as five ASCII characters
    uint8_t checksum;       // 0x05: Checksum of the Entry Point Structure
    uint8_t length;         // 0x06: Length of the Entry Point Structure
    uint8_t major;          // 0x07: Major version of this specification
    uint8_t minor;          // 0x08: Minor version of this specification
    uint8_t docrev;         // 0x09: docrev of this specification implemented in the table structures
    uint32_t strmax;        // 0x0c: Maximum size of SMBIOS Structure Table
    uint64_t straddr;       // 0x10: Physical address of SMBIOS Structure Table
} __packed;

enum dmi_device_type {
    DMI_DEVICE_ANY          = 0,
    DMI_DEVICE_OTHER        = 1,
    DMI_DEVICE_UNKNOWN      = 2,
    DMI_DEVICE_VIDEO        = 3,
    DMI_DEVICE_SCSI         = 4,
    DMI_DEVICE_ETHERNET     = 5,
    DMI_DEVICE_TOKENRING    = 6,
    DMI_DEVICE_SOUND        = 7,
    DMI_DEVICE_PATA         = 8,
    DMI_DEVICE_SATA         = 9,
    DMI_DEVICE_SAS          = 10,
};

enum dmi_entry_type {
    DMI_ENTRY_BIOS                      = 0,
    DMI_ENTRY_SYSTEM                    = 1,
    DMI_ENTRY_BASEBOARD                 = 2,
    DMI_ENTRY_CHASSIS                   = 3,
    DMI_ENTRY_PROCESSOR                 = 4,
    DMI_ENTRY_MEM_CONTROLLER            = 5,
    DMI_ENTRY_MEM_MODULE                = 6,
    DMI_ENTRY_CACHE                     = 7,
    DMI_ENTRY_PORT_CONNECTOR            = 8,
    DMI_ENTRY_SYSTEM_SLOT               = 9,
    DMI_ENTRY_ONBOARD_DEVICE            = 10,
    DMI_ENTRY_OEMSTRINGS                = 11,
    DMI_ENTRY_SYSCONF                   = 12,
    DMI_ENTRY_BIOS_LANG                 = 13,
    DMI_ENTRY_GROUP_ASSOC               = 14,
    DMI_ENTRY_SYSTEM_EVENT_LOG          = 15,
    DMI_ENTRY_PHYS_MEM_ARRAY            = 16,
    DMI_ENTRY_MEM_DEVICE                = 17,
    DMI_ENTRY_32_MEM_ERROR              = 18,
    DMI_ENTRY_MEM_ARRAY_MAPPED_ADDR     = 19,
    DMI_ENTRY_MEM_DEV_MAPPED_ADDR       = 20,
    DMI_ENTRY_BUILTIN_POINTING_DEV      = 21,
    DMI_ENTRY_PORTABLE_BATTERY          = 22,
    DMI_ENTRY_SYSTEM_RESET              = 23,
    DMI_ENTRY_HW_SECURITY               = 24,
    DMI_ENTRY_SYSTEM_POWER_CONTROLS     = 25,
    DMI_ENTRY_VOLTAGE_PROBE             = 26,
    DMI_ENTRY_COOLING_DEV               = 27,
    DMI_ENTRY_TEMP_PROBE                = 28,
    DMI_ENTRY_ELECTRICAL_CURRENT_PROBE  = 29,
    DMI_ENTRY_OOB_REMOTE_ACCESS         = 30,
    DMI_ENTRY_BIS_ENTRY                 = 31,
    DMI_ENTRY_SYSTEM_BOOT               = 32,
    DMI_ENTRY_MGMT_DEV                  = 33,
    DMI_ENTRY_MGMT_DEV_COMPONENT        = 34,
    DMI_ENTRY_MGMT_DEV_THRES            = 35,
    DMI_ENTRY_MEM_CHANNEL               = 36,
    DMI_ENTRY_IPMI_DEV                  = 37,
    DMI_ENTRY_SYS_POWER_SUPPLY          = 38,
    DMI_ENTRY_ADDITIONAL                = 39,
    DMI_ENTRY_ONBOARD_DEV_EXT           = 40,
    DMI_ENTRY_MGMT_CONTROLLER_HOST      = 41,
    DMI_ENTRY_INACTIVE                  = 126,
    DMI_ENTRY_END_OF_TABLE              = 127,
};

struct dmi_header {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
} __packed;

enum dmi_bios {
    DMI_HD_BIOS_VENDOR      = 0x04, // String number of the BIOS Vendor’s Name
    DMI_HD_BIOS_VERSION     = 0x05, // String number of the BIOS Version
    DMI_HD_BIOS_ADDRESS     = 0x06, // Segment location of BIOS starting address
    DMI_HD_BIOS_DATE        = 0x08, // String number of the BIOS release date
    DMI_HD_BIOS_ROM_SIZE    = 0x09, // Size of the physical device containing the BIOS (64K * (n+1))
    DMI_HD_BIOS_FEATURES    = 0x0a, // Defines which functions the BIOS supports
    DMI_HD_BIOS_EFEATURES   = 0x12, // Optional space reserved for future supported functions
    DMI_HD_BIOS_MAJOR       = 0x14, // Identifies the major release of the System BIOS
    DMI_HD_BIOS_MINOR       = 0x15, // Identifies the minor release of the System BIOS
    DMI_HD_EC_MAJOR         = 0x16, // Identifies the major release of the EC firmware
    DMI_HD_EC_MINOR         = 0x17, // Identifies the minor release of the EC firmware
    DMI_HD_EBIOS_ROM_SIZE   = 0x18, // Extended size of the physical device containing the BIOS
};

enum dmi_sys {
    DMI_HD_SYS_MANUFACT = 0x04, // Number of null-terminated string
    DMI_HD_SYS_PRODUCT  = 0x05, // Number of null-terminated string
    DMI_HD_SYS_VERSION  = 0x06, // Number of null-terminated string
    DMI_HD_SYS_SERIAL   = 0x07, // Number of null-terminated string
    DMI_HD_SYS_UUID     = 0x08, // Universal unique ID number
    DMI_HD_SYS_WEAK     = 0x18, // Identifies the event that caused the system to power up.
    DMI_HD_SYS_SKU      = 0x19, // Number of null-terminated string
    DMI_HD_SYS_FAMILY   = 0x1a, // Number of null-terminated string
};

struct dmi_node {
    const char *name;
    enum dmi_device_type type;
    struct slist_head list;
};

extern char *dmi_string_index(const struct dmi_header *dmi, int index);
extern void dmi_region_byte(struct dmi_header *dmi, int index, enum dmi_region_type region);
extern void dmi_region_word(struct dmi_header *dmi, int index, enum dmi_region_type region);
extern void dmi_region_string(struct dmi_header *dmi, int index, enum dmi_region_type region);
extern bool dmi_match_one(enum dmi_region_type type, const char *name);
extern const void *dmi_match(struct dmi_deivce_id *dmi);

#endif  /* _DRIVER_DMI_H_ */
