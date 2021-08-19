/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2012-2013 Samsung Electronics Co., Ltd.
 */

#ifndef _FS_EXFAT_H_
#define _FS_EXFAT_H_

#include <types.h>

#define BOOTSEC_JUMP_BOOT_LEN   3
#define BOOTSEC_FS_NAME_LEN     8
#define BOOTSEC_OLDBPB_LEN      53
#define EXFAT_FILE_NAME_LEN     15

struct boot_sector {
    uint8_t jmp_boot[BOOTSEC_JUMP_BOOT_LEN];
    uint8_t fs_name[BOOTSEC_FS_NAME_LEN];
    uint8_t must_be_zero[BOOTSEC_OLDBPB_LEN];
    le64    partition_offset;
    le64    vol_length;
    le32    fat_offset;
    le32    fat_length;
    le32    clu_offset;
    le32    clu_count;
    le32    root_cluster;
    le32    vol_serial;
    uint8_t fs_revision[2];
    le16    vol_flags;
    uint8_t sect_size_bits;
    uint8_t sect_per_clus_bits;
    uint8_t num_fats;
    uint8_t drv_sel;
    uint8_t percent_in_use;
    uint8_t reserved[7];
    uint8_t boot_code[390];
    le16    signature;
} __packed;

struct exfat_dentry {
    uint8_t type;
    union {
        struct {
            uint8_t num_ext;
            le16 checksum;
            le16 attr;
            le16 reserved1;
            le16 create_time;
            le16 create_date;
            le16 modify_time;
            le16 modify_date;
            le16 access_time;
            le16 access_date;
            uint8_t create_time_cs;
            uint8_t modify_time_cs;
            uint8_t create_tz;
            uint8_t modify_tz;
            uint8_t access_tz;
            uint8_t reserved2[7];
        } __packed file; /* file directory entry */
        struct {
            uint8_t flags;
            uint8_t reserved1;
            uint8_t name_len;
            le16 name_hash;
            le16 reserved2;
            le64 valid_size;
            le32 reserved3;
            le32 start_clu;
            le64 size;
        } __packed stream; /* stream extension directory entry */
        struct {
            uint8_t flags;
            le16 unicode_0_14[EXFAT_FILE_NAME_LEN];
        } __packed name; /* file name directory entry */
        struct {
            uint8_t flags;
            uint8_t reserved[18];
            le32 start_clu;
            le64 size;
        } __packed bitmap; /* allocation bitmap directory entry */
        struct {
            uint8_t reserved1[3];
            le32 checksum;
            uint8_t reserved2[12];
            le32 start_clu;
            le64 size;
        } __packed upcase; /* up-case table directory entry */
    } __packed dentry;
} __packed;

#define BOOT_SIGNATURE      0xAA55
#define EXBOOT_SIGNATURE    0xAA550000
#define STR_EXFAT           "EXFAT   "    /* size should be 8 */

#define EXFAT_MAX_FILE_LEN  255

#define VOLUME_DIRTY        0x0002
#define MEDIA_FAILURE       0x0004

#define EXFAT_EOF_CLUSTER   0xFFFFFFFFu
#define EXFAT_BAD_CLUSTER   0xFFFFFFF7u
#define EXFAT_FREE_CLUSTER  0
/* Cluster 0, 1 are reserved, the first cluster is 2 in the cluster heap. */
#define EXFAT_RESERVED_CLUSTERS     2
#define EXFAT_FIRST_CLUSTER         2
#define EXFAT_DATA_CLUSTER_COUNT(sbi)    \
    ((sbi)->num_clusters - EXFAT_RESERVED_CLUSTERS)

/* AllocationPossible and NoFatChain field in GeneralSecondaryFlags Field */
#define ALLOC_FAT_CHAIN     0x01
#define ALLOC_NO_FAT_CHAIN  0x03

#define DENTRY_SIZE         32 /* directory entry size */
#define DENTRY_SIZE_BITS    5
/* exFAT allows 8388608(256MB) directory entries */
#define MAX_EXFAT_DENTRIES  8388608

/* dentry types */
#define EXFAT_UNUSED        0x00    /* end of directory */
#define EXFAT_DELETE        (~0x80)
#define IS_EXFAT_DELETED(x) ((x) < 0x80) /* deleted file (0x01~0x7F) */
#define EXFAT_INVAL         0x80    /* invalid value */
#define EXFAT_BITMAP        0x81    /* allocation bitmap */
#define EXFAT_UPCASE        0x82    /* upcase table */
#define EXFAT_VOLUME        0x83    /* volume label */
#define EXFAT_FILE          0x85    /* file or dir */
#define EXFAT_GUID          0xA0
#define EXFAT_PADDING       0xA1
#define EXFAT_ACLTAB        0xA2
#define EXFAT_STREAM        0xC0    /* stream entry */
#define EXFAT_NAME          0xC1    /* file name entry */
#define EXFAT_ACL           0xC2    /* stream entry */

#define IS_EXFAT_CRITICAL_PRI(x)    (x < 0xA0)
#define IS_EXFAT_BENIGN_PRI(x)      (x < 0xC0)
#define IS_EXFAT_CRITICAL_SEC(x)    (x < 0xE0)

/* checksum types */
#define CS_DIR_ENTRY        0
#define CS_BOOT_SECTOR      1
#define CS_DEFAULT          2

/* file attributes */
#define ATTR_READONLY       0x0001
#define ATTR_HIDDEN         0x0002
#define ATTR_SYSTEM         0x0004
#define ATTR_VOLUME         0x0008
#define ATTR_SUBDIR         0x0010
#define ATTR_ARCHIVE        0x0020

#define ATTR_RWMASK     (ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME | \
                         ATTR_SUBDIR | ATTR_ARCHIVE)

#endif /* _FS_EXFAT_H_ */
