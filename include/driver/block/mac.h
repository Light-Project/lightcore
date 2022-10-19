/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_MAC_H_
#define _DRIVER_BLOCK_MAC_H_

#include <types.h>

struct mac_driver {
	be16 signature;
	be16 block_size;
	be32 block_count;
};

struct mac_partition {
    be16 signature;
    be16 res1;
    be32 map_count;
    be32 start_block;
    be32 block_count;
    uint8_t name[32];
    uint8_t type[32];
    be32 data_start;
    be32 data_count;
    be32 status;
    be32 boot_start;
    be32 boot_size;
    be32 boot_load;
    be32 boot_load2;
    be32 boot_entry;
    be32 boot_entry2;
    be32 boot_cksum;
    uint8_t processor[16];
};

#define MAC_DRIVER_MAGIC 0x4552
#define MAC_PARTITION_MAGIC	0x504d
#define MAC_BOOTABLE 8

#endif  /* _DRIVER_BLOCK_MAC_H_ */
