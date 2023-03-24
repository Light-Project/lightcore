/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_ROMFS_H_
#define _FS_ROMFS_H_

#include <types.h>

#define ROMFS_MAGIC "-rom1fs-"
#define ROMFS_MAXFN 128

struct romfs_inode {
    be32 next;
    be32 spec;
    be32 size;
    be32 checksum;
    char name[0];
} __packed;

struct romfs_superblock {
    char magic[8];
    be32 size;
    be32 checksum;
    char name[0];
} __packed;

enum romfs_type {
    ROMFH_TYPE  = 7,
    ROMFH_HRD   = 0,
    ROMFH_DIR   = 1,
    ROMFH_REG   = 2,
    ROMFH_SYM   = 3,
    ROMFH_BLK   = 4,
    ROMFH_CHR   = 5,
    ROMFH_SCK   = 6,
    ROMFH_FIF   = 7,
    ROMFH_EXEC  = 8,
};

#define ROMFH_SIZE  16
#define ROMFH_PAD   (ROMFH_SIZE - 1)
#define ROMFH_MASK  (~ROMFH_PAD)

#endif /* _FS_ROMFS_H_ */
