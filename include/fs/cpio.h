/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_CPIO_H_
#define _FS_CPIO_H_

#include <types.h>

#define CPIO_MAGIC  "070701"

struct cpio_inode {
    char magic[6];      /* 0x00: "070701" for newc */
    char ino[8];        /* 0x06: */
    char mode[8];       /* 0x0e: */
    char uid[8];        /* 0x16: */
    char gid[8];        /* 0x1e: */
    char nlink[8];      /* 0x26: */
    char mtime[8];      /* 0x2e: */
    char filesize[8];   /* 0x36: must be 0 for fifos and directories */
    char devmajor[8];   /* 0x3e: */
    char devminor[8];   /* 0x46: */
    char rdevmajor[8];  /* 0x4e: only valid for char and block special files */
    char rdevminor[8];  /* 0x56: only valid for char and block special files */
    char namesize[8];   /* 0x5e: count includes terminating NUL in pathname */
    char check[8];      /* 0x66: 0 for "new" portable format; for CRC format the sum of all the bytes in the file */
    char name[0];
} __packed;

/*
 * A cpio archive consists of a sequence of files.
 * Each file has a 76 byte header,
 * a variable length, NUL terminated filename,
 * and variable length file data.
 * A header for a filename "TRAILER!!!" indicates the end of the archive.
 */

#define CPIO_TRAILER_NAME "TRAILER!!!"

#endif /* _FS_CPIO_H_ */
