/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_DIRENT_H_
#define _FS_DIRENT_H_

#include <list.h>

struct dirent {
    const char *name;
    unsigned int len;

    struct dir_cache *parent;   /* parent directory */
    struct list_head child;     /* child directory */
    struct list_head sibling;   /* child list */

    struct superblock *sb;
};

#endif  /* _FS_DIRENT_H_ */
