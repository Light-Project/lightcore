/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_DCACHE_H_
#define _FS_DCACHE_H_

#include <list.h>

struct dcache {
    struct dir_cache *parent;   /* parent directory */
    struct list_head subdir;    /* child directory */
    struct list_head list;      /* child list */

    struct superblock *sb;
};

#endif  /* _FS_DCACHE_H_ */
