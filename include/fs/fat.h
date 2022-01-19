/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_FAT_H_
#define _FS_FAT_H_

#include <types.h>

#define FAT_NAME    11

#define FAT_BAD12   0xff7
#define FAT_BAD16   0xfff7
#define FAT_BAD32   0x0ffffff7

#define FAT_EOF12   0xfff
#define FAT_EOF16   0xffff
#define FAT_EOF32   0x0fffffff

struct fat_boot_sector {
    uint8_t ignored[3];     /* 0x00: Boot strap short or near jump */
    uint8_t system_id[8];   /* 0x03: Name - can be used to special case partition manager volumes */
    uint8_t sector_size[2]; /* 0x0b: bytes per logical sector */
    uint8_t sec_per_clus;   /* 0x0d: sectors/cluster */
    le16 reserved;          /* 0x0e: reserved sectors */
    uint8_t fats;           /* 0x10: number of FATs */
    uint8_t dir_entries[2]; /* 0x11: root directory entries */
    uint8_t sectors[2];     /* 0x13: number of sectors */
    uint8_t media;          /* 0x15: media code */
    le16 fat_length;        /* 0x16: sectors per FAT */
    le16 secs_track;        /* 0x18: sectors per track */
    le16 heads;             /* 0x1a: number of heads */
    le32 hidden;            /* 0x1c: hidden sectors (unused) */
    le32 total_sect;        /* 0x20: number of sectors (if sectors == 0) */

    union {
        struct {
            uint8_t drive_number;           /* 0x24: Physical drive number */
            uint8_t state;                  /* 0x25: undocumented, but used for mount state. */
            uint8_t signature;              /* 0x26: extended boot signature */
            uint8_t vol_id[4];              /* 0x27: volume ID */
            uint8_t vol_label[FAT_NAME];    /* 0x2b: volume label */
            uint8_t fs_type[8];             /* 0x36: file system type */
        } fat16;

        struct {
            le32 length;                    /* 0x24: sectors/FAT */
            le16 flags;                     /* 0x28: bit 8: fat mirroring, low 4: active fat */
            uint8_t version[2];             /* 0x2a: major, minor filesystem version */
            le32 root_cluster;              /* 0x2c: first cluster in root directory */
            le16 info_sector;               /* 0x30: filesystem info sector */
            le16 backup_boot;               /* 0x34: backup boot sector */
            le16 reserved2[6];              /* 0x36: unused */

            /* Extended BPB Fields for FAT32 */
            uint8_t drive_number;           /* 0x40: Physical drive number */
            uint8_t state;                  /* 0x41: undocumented, but used for mount state. */
            uint8_t signature;              /* 0x42: extended boot signature */
            uint8_t vol_id[4];              /* 0x43: volume ID */
            uint8_t vol_label[FAT_NAME];    /* 0x47: volume label */
            uint8_t fs_type[8];             /* 0x52: file system type */
        } fat32;
    };
} __packed;

struct fat_dir_entry {
    uint8_t name[FAT_NAME];     /* 0x00: name and extension */
    uint8_t attr;               /* 0x0b: attribute bits */
    uint8_t lcase;              /* 0x0c: Case for base and extension */
    uint8_t ctime_cs;           /* 0x0d: Creation time, centiseconds (0-199) */
    le16 ctime;                 /* 0x0e: Creation time */
    le16 cdate;                 /* 0x10: Creation date */
    le16 adate;                 /* 0x12: Last access date */
    le16 starthi;               /* 0x14: High 16 bits of cluster in FAT32 */
    le16 time, date, start;     /* 0x16: time, date and first cluster */
    le32 size;                  /* 0x1c: file size (in bytes) */
} __packed;

#endif  /* _FS_FAT_H_ */
