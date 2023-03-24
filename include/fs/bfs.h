/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_BFS_H_
#define _FS_BFS_H_

#include <types.h>

#define BFS_MAGIC       0x1BADFACE
#define BFS_VTYPE_REG   1L
#define BFS_VTYPE_DIR   2L

struct bfs_inode {
	le16 ino;               /* 0x00:  */
	uint16_t unused;        /* 0x02:  */
	le32 sblock;            /* 0x00:  */
	le32 eblock;            /* 0x00:  */
	le32 eoffset;           /* 0x00:  */
	le32 vtype;             /* 0x00:  */
	le32 mode;              /* 0x00:  */
	le32 uid;               /* 0x00:  */
	le32 gid;               /* 0x00:  */
	le32 nlink;             /* 0x00:  */
	le32 atime;             /* 0x00:  */
	le32 mtime;             /* 0x00:  */
	le32 ctime;             /* 0x00:  */
	uint32_t padding[4];    /* 0x00:  */
} __packed;

struct bfs_superblock {
    le32 magic;             /* 0x00:  */
    le32 start;             /* 0x00:  */
    le32 end;               /* 0x00:  */
    le32 from;              /* 0x00:  */
    le32 to;                /* 0x00:  */
    int32_t bfrom;          /* 0x00:  */
    int32_t bto;            /* 0x00:  */
	char fsname[6];         /* 0x00:  */
	char volume[6];         /* 0x00:  */
	uint32_t padding[118];  /* 0x00:  */
} __packed;

#endif /* _FS_BFS_H_ */
