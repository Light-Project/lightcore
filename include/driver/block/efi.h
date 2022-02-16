/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_BLOCK_EFI_H_
#define _DRIVER_BLOCK_EFI_H_

#include <types.h>
#include <uuid.h>

typedef guid_t efi_guid_t;

struct efi_entry_att {
    uint64_t required_to_function:1;
    uint64_t reserved:47;
    uint64_t type_guid_specific:16;
} __packed;

struct efi_entry {
    efi_guid_t type_guid;               /* 0x00: partition type guid */
    efi_guid_t unique_guid;             /* 0x10: unique partition guid */
    le64 start_lba;                     /* 0x18: starting lba */
    le64 end_lba;                       /* 0x20: ending lba */
    struct efi_entry_att attributes;    /* 0x28: */
    le16 name[72 / sizeof(le16)];       /* 0x30: */
} __packed;

struct efi_sector {
    struct efi_entry entry[4];
};

struct efi_head {
    uint8_t magic[8];                   /* 0x00: gpt signature */
    le32 revision;                      /* 0x08: */
    le32 header_size;                   /* 0x0c: */
    le32 header_crc32;                  /* 0x10: */
    le32 reserved1;                     /* 0x14: */
    le64 my_lba;                        /* 0x18: */
    le64 alternate_lba;                 /* 0x20: */
    le64 first_usable_lba;              /* 0x28: */
    le64 last_usable_lba;               /* 0x30: */
    efi_guid_t disk_guid;               /* 0x38: */
    le64 partition_entry_lba;           /* 0x48: */
    le32 num_partition_entries;         /* 0x50: */
    le32 sizeof_partition_entry;        /* 0x54: */
    le32 partition_entry_array_crc32;   /* 0x58: */
    uint8_t reserved2[442];             /* 0x5c: */
} __packed;

#define EFI_MAGIC "EFI PART"
#define EFI_HEAD_LBA 1
#define EFI_SECTOR_LBA 2

#endif  /* _DRIVER_BLOCK_EFI_H_ */
