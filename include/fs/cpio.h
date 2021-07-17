/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_CPIO_H_
#define _FS_CPIO_H_

#include <types.h>

struct cpio_inode_old {
    uint16_t magic;         /* 0x00: 0xc701 for odc */
    uint16_t dev;
    uint16_t ino;
    uint16_t mode;
    uint16_t uid;
    uint16_t gid;
    uint16_t nlink;
    uint16_t rdev;
    uint16_t mtimes[2];
    uint16_t namesize;
    uint16_t filesizes[2];
} __packed;

struct cpio_inode_odc {
    uint8_t magic[6];       /* 0x00: "070707" for odc */
    uint8_t dev[6];
    uint8_t ino[6];
    uint8_t mode[6];
    uint8_t uid[6];
    uint8_t gid[6];
    uint8_t nlink[6];
    uint8_t rdev[6];
    uint8_t mtime[11];
    uint8_t namesize[6];
    uint8_t filesize[11];
} __packed;

struct cpio_inode_newc {
    uint8_t magic[6];       /* 0x00: "070701" for newc */
    uint8_t ino[8];         /* 0x06: */
    uint8_t mode[8];        /* 0x0e: */
    uint8_t uid[8];         /* 0x16: */
    uint8_t gid[8];         /* 0x1e: */
    uint8_t nlink[8];       /* 0x26: */
    uint8_t mtime[8];       /* 0x2e: */
    uint8_t filesize[8];    /* 0x36: must be 0 for FIFOs and directories */
    uint8_t devmajor[8];    /* 0x3e: */
    uint8_t devminor[8];    /* 0x46: */
    uint8_t rdevmajor[8];   /* 0x4e: only valid for chr and blk special files */
    uint8_t rdevminor[8];   /* 0x56: only valid for chr and blk special files */
    uint8_t namesize[8];    /* 0x5e: count includes terminating NUL in pathname */
    uint8_t check[8];       /* 0x66: 0 for "new" portable format; for CRC format the sum of all the bytes in the file */
} __packed;

#define CP_IFMT 0170000		/* Mask for all file type bits.  */

enum cpio_mode {
    CP_IFBLK    = 0060000,  /* blockdev */
    CP_IFCHR    = 0020000,  /* chardev */
    CP_IFDIR    = 0040000,  /* directory */
    CP_IFREG    = 0100000,  /* regular file */
    CP_IFIFO    = 0010000,  /* FIFO */
    CP_IFLNK    = 0120000,  /* symlink */
    CP_IFSOCK   = 0140000,  /* socket */
    CP_IFNWK    = 0110000,  /* network */
};

/* A cpio archive consists of a sequence of files.
   Each file has a 76 byte header,
   a variable length, NUL terminated filename,
   and variable length file data.
   A header for a filename "TRAILER!!!" indicates the end of the archive.  */

#define CPIO_TRAILER_NAME "TRAILER!!!"

#endif  /* _FS_CPIO_H_ */
