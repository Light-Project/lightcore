/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_EFI_H_
#define _DRIVER_BLOCK_EFI_H_

#include <types.h>
#include <uuid.h>

struct efi_entry_att {
    uint64_t required_to_function:1;
    uint64_t reserved:47;
    uint64_t type_guid_specific:16;
} __packed;

struct efi_entry {
    guid_t type_guid;                   /* 0x00: Partition type GUID (mixed endian) */
    guid_t unique_guid;                 /* 0x10: Unique partition GUID (mixed endian) */
    le64 start_lba;                     /* 0x20: First LBA (little endian) */
    le64 end_lba;                       /* 0x28: Last LBA (inclusive, usually odd) */
    struct efi_entry_att attributes;    /* 0x30: Attribute flags (e.g. bit 60 denotes read-only) */
    le16 name[72 / sizeof(le16)];       /* 0x38: Partition name (36 UTF-16LE code units) */
} __packed;

struct efi_sector {
    struct efi_entry entry[4];
} __packed;

struct efi_head {
    uint8_t magic[8];                   /* 0x00: Signature */
    le32 revision;                      /* 0x08: Revision 1.0 (00h 00h 01h 00h) for UEFI 2.8 */
    le32 header_size;                   /* 0x0c: Header size in little endian */
    le32 header_crc32;                  /* 0x10: CRC32 of header in little endian */
    le32 reserved1;                     /* 0x14: Reserved; must be zero */
    le64 my_lba;                        /* 0x18: Current LBA (location of this header copy) */
    le64 alternate_lba;                 /* 0x20: Backup LBA (location of the other header copy) */
    le64 first_usable_lba;              /* 0x28: First usable LBA for partitions (primary partition table last LBA + 1) */
    le64 last_usable_lba;               /* 0x30: Last usable LBA (secondary partition table first LBA − 1) */
    guid_t disk_guid;                   /* 0x38: Disk GUID in mixed endian */
    le64 partition_entry_lba;           /* 0x48: Starting LBA of array of partition entries (usually 2 for compatibility) */
    le32 num_partition_entries;         /* 0x50: Number of partition entries in array */
    le32 sizeof_partition_entry;        /* 0x54: Size of a single partition entry (usually 80h or 128) */
    le32 partition_entry_array_crc32;   /* 0x58: CRC32 of partition entries array in little endian */
    uint8_t reserved2[420];             /* 0x5c: Reserved; must be zeroes for the rest of the block */
} __packed;

#define EFI_MAGIC "EFI PART"
#define EFI_HEAD_LBA 1
#define EFI_SECTOR_LBA 2

#endif /* _DRIVER_BLOCK_EFI_H_ */
