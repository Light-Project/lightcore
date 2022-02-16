/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_MSDOS_H_
#define _DRIVER_BLOCK_MSDOS_H_

#include <types.h>

struct msdos_dpt {
    uint8_t flag;               /* 0x0000: 0x80 active partition */
    uint8_t head;               /* 0x0001: start head */
    uint8_t sec;                /* 0x0002: start sector */
    uint8_t cyl;                /* 0x0003: start cylinder */
    uint8_t type;               /* 0x0004: partition type */
    uint8_t end_head;           /* 0x0005: end head */
    uint8_t end_sec;            /* 0x0006: end sector */
    uint8_t end_cyl;            /* 0x0007: end cylinder */
    le32 lba;                   /* 0x0008: start lba */
    le32 size;                  /* 0x000c: partition size */
} __packed;

struct msdos_head {
    uint8_t mbr[446];           /* 0x0000: boot code */
    struct msdos_dpt dpt[4];    /* 0x01c0: part entry */
    be16 magic;                 /* 0x01fe: msdos disk magic */
} __packed;

enum msdos_type {
    MSDOS_DOS_EXT_PART          = 0x05,
    MSDOS_WIN98_EXT_PART        = 0x0f,
    MSDOS_DM6_AUX1PART          = 0x51,
    MSDOS_DM6_AUX3PART          = 0x53,
    MSDOS_DM6_PART              = 0x54,
    MSDOS_EZD_PART              = 0x55,
    MSDOS_UNIXWARE_PART         = 0x63,
    MSDOS_MINIX_PART            = 0x81,
    MSDOS_LINUX_EXT_PART        = 0x85,
    MSDOS_LINUX_DATA_PART       = 0x83,
    MSDOS_LINUX_LVM_PART        = 0x8e,
    MSDOS_LINUX_RAID_PART       = 0xfd,
    MSDOS_FREEBSD_PART          = 0xa5,
    MSDOS_OPENBSD_PART          = 0xa6,
    MSDOS_NETBSD_PART           = 0xa9,
    MSDOS_BSDI_PART             = 0xb7,
};

#define MSDOS_MAGIC 0x55aa

#endif  /* _DRIVER_BLOCK_MSDOS_H_ */
