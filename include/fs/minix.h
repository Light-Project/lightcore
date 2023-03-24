/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_MINIX_H_
#define _FS_MINIX_H_

#include <types.h>

struct minix_inode {
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t time;
    uint8_t  gid;
    uint8_t  nlinks;
    uint16_t zone[9];
} __packed;

struct minix2_inode {
    uint16_t mode;
    uint16_t nlinks;
    uint16_t uid;
    uint16_t gid;
    uint32_t size;
    uint32_t atime;
    uint32_t mtime;
    uint32_t ctime;
    uint32_t zone[10];
} __packed;

struct minix_dirent {
    uint16_t inode;
    char name[0];
} __packed;

struct minix3_dirent {
    uint32_t inode;
    char name[0];
} __packed;

struct minix_superblock {
    uint16_t ninodes;
    uint16_t nzones;
    uint16_t imap_blocks;
    uint16_t zmap_blocks;
    uint16_t firstdatazone;
    uint16_t log_zone_size;
    uint32_t max_size;
    uint16_t magic;
    uint16_t state;
    uint32_t zones;
} __packed;

struct minix3_superblock {
    uint32_t ninodes;
    uint16_t pad0;
    uint16_t imap_blocks;
    uint16_t zmap_blocks;
    uint16_t firstdatazone;
    uint16_t log_zone_size;
    uint16_t pad1;
    uint32_t max_size;
    uint32_t zones;
    uint16_t magic;
    uint16_t pad2;
    uint16_t blocksize;
    uint8_t  disk_version;
} __packed;

#define MINIX_SUPER_MAGIC   0x137f
#define MINIX_SUPER_MAGIC2  0x138f
#define MINIX2_SUPER_MAGIC  0x2468
#define MINIX2_SUPER_MAGIC2 0x2478
#define MINIX3_SUPER_MAGIC  0x4d5a

#define MINIX_VALID_FS  0x0001
#define MINIX_ERROR_FS  0x0002

#define MINIX_ROOT_INO  1

#endif /* _FS_MINIX_H_ */
