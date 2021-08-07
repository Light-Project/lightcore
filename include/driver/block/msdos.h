/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_MSDOS_H_
#define _DRIVER_BLOCK_MSDOS_H_

#include <types.h>

struct msdos_dpt {
    uint8_t flag;       /* 0x80 active partition */
    uint8_t head;       /* start head */
    uint8_t sec;        /* start sector */
    uint8_t cyl;        /* start cylinder */
    uint8_t type;       /* partition type */
    uint8_t end_head;   /* end head */
    uint8_t end_sec;    /* end sector */
    uint8_t end_cyl;    /* end cylinder */
    uint32_t lba;       /* start lba */
    uint32_t size;      /* partition size */
} __packed;

struct msdos_head {
    uint8_t mbr[446];
    struct msdos_dpt dpt[4];
    uint16_t magic;
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
