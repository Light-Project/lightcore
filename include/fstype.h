/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_TYPE_H_
#define _FS_TYPE_H_

struct dirent;
struct vflmount;

/**
 * path -
 *
 */
struct path {
    struct vflmount *mount;
    struct dirent *dirent;
};

/**
 * filesystem - filesystem state
 * @root: root path
 * @pwd: print work directory
 */
struct filesystem {
    struct path root;
    struct path pwd;
};

#endif /* _FS_TYPE_H_ */
