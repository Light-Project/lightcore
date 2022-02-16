/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_SUN_H_
#define _DRIVER_BLOCK_SUN_H_

#include <types.h>
#include <uuid.h>

struct sun_info {
    be16 id;                    /*  */
    be16 flags;                 /*  */
} __packed;

struct sun_vtoc {
    be32 version;               /* Layout version */
    uint8_t volume[8];          /* Volume name */
    be16 nparts;                /* Number of partitions */
    struct sun_info info[8];    /* Partition hdrs, sec 2 */
    be16 padding;               /* Alignment padding */
    be32 bootinfo[3];           /* Info needed by mboot */
    be32 sanity;                /* To verify vtoc sanity */
    be32 reserved[10];          /* Free space */
    be32 timestamp[8];          /* Partition timestamp */
} __packed;

struct sun_part {
    be32 start_cylinder;        /*  */
    be32 num_sectors;           /*  */
} __packed;

struct sun_head {
    uint8_t info[128];          /* Informative text string */
    struct sun_vtoc vtoc;       /*  */
    be32 write_reinstruct;      /* sectors to skip, writes */
    be32 read_reinstruct;       /* sectors to skip, reads */
    uint8_t spare[148];         /* Padding */
    be16 rspeed;                /* Disk rotational speed */
    be16 pcylcount;             /* Physical cylinder count */
    be16 sparecyl;              /* extra sects per cylinder */
    be16 obs1;                  /* gap1 */
    be16 obs2;                  /* gap2 */
    be16 ilfact;                /* Interleave factor */
    be16 ncyl;                  /* Data cylinder count */
    be16 nacyl;                 /* Alt. cylinder count */
    be16 ntrks;                 /* Tracks per cylinder */
    be16 nsect;                 /* Sectors per track */
    be16 obs3;                  /* bhead - Label head offset */
    be16 obs4;                  /* ppart - Physical Partition */
    struct sun_part part[8];    /*  */
    be16 magic;                 /* Magic number */
    be16 csum;                  /* Label xor'd checksum */
} __packed;

#define SUN_LABEL_MAGIC     0xdabe
#define SUN_VTOC_SANITY     0x600ddeee

#endif  /* _DRIVER_BLOCK_SUN_H_ */
