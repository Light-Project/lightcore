/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_DIRENT_H_
#define _FS_DIRENT_H_

#include <list.h>

/**
 * dirent -
 * @parent: parent directory
 * @child: child directory list
 * @sibling: child list by @child
 * @sb: root of the dentry tree
 * @inode:
 */
struct dirent {
    const char *name;
    unsigned int len;

    struct dirent *parent;
    struct list_head child;
    struct list_head sibling;

    struct superblock *sb;
    struct inode *inode;
};

#endif /* _FS_DIRENT_H_ */
