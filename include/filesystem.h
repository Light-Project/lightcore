/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <error.h>
#include <bits.h>
#include <list.h>
#include <time.h>
#include <fstype.h>
#include <fsdev.h>
#include <fs/dirent.h>
#include <lightcore/filesystem.h>

struct vmem_area;

typedef enum fmode {
    FMODE_RDONLY    = BIT(0),
    FMODE_WRONLY    = BIT(1),
    FMODE_RW        = FMODE_RDONLY | FMODE_WRONLY,

    FMODE_LSEEK     = BIT(3),
    FMODE_PREAD     = BIT(4),
    FMODE_EXEC      = BIT(5),
} fmode_t;

enum mount_flags {
    MOUNT_RDONLY    = BIT(0),
    MOUNT_WRONLY    = BIT(1),
    MOUNT_RW        = MOUNT_RDONLY | MOUNT_WRONLY,
};

/**
 * file - describe an file of vfl
 *
 */
struct file {
    enum fmode fmode;
    const char *path;
    loff_t offset;

    struct file_ops *ops;
    void *data;
};

/**
 * inode - describe an inode of vfl
 * @atime: access time
 * @mtime: modify time
 * @ctime: change time
 * @lru: inode lru list
 */
struct inode {
    umode_t mode;
    uid_t uid;
    gid_t gid;

    struct timespec atime;
    struct timespec mtime;
    struct timespec ctime;

    struct superblock *sb;
    struct inode_ops *ops;
    struct file_ops *fops;
    struct list_head lru;
};

/**
 * superblock - describe an superblock of vfl
 *
 *
 */
struct superblock {
    struct list_head mount_list;
    struct filesystem_type *fs_type;
    struct fsdev *fsdev;

    struct dirent *root;
    struct list_head inode;
    const struct superblock_ops *ops;
};

/**
 * file_ops - file operations
 * @open:
 * @close:
 * @read:
 * @write:
 * @ioctl:
 * @mmap:
 */
struct file_ops {
    state (*open)(struct inode *inode, struct file *file);
    state (*close)(struct inode *inode, struct file *file);
    state (*read)(struct file *file, void *dest, size_t len, loff_t *off);
    state (*write)(struct file *file, void *src, size_t len, loff_t *off);
    state (*ioctl)(struct file *file, unsigned int cmd, unsigned long arg);
    state (*mmap)(struct file *file, struct vmem_area *vmem);
};

/**
 * inode_ops - inode operations
 * @lookup:
 * @mkdir:
 * @rmdir:
 * @readlink:
 * @symlink:
 * @link:
 * @unlink:
 */
struct inode_ops {
    struct dirent *(*lookup)(struct inode *inode, struct dirent *dir, unsigned int);
    state (*mkdir)(struct dirent *dir, struct inode *inode, umode_t mode);
    state (*rmdir)(struct dirent *dir, struct inode *inode);
    state (*readlink)(struct dirent *dir, char *, int);
    state (*symlink)(struct inode *inode, struct dirent *dir, const char *);
    state (*link)(struct dirent *dir, struct inode *inode,struct dirent *);
    state (*unlink)(struct inode *inode, struct dirent *dir);
};

/**
 * superblock_ops - superblock operations
 * @inode_alloc:
 * @inode_free:
 * @sync:
 */
struct superblock_ops {
    struct inode *(*inode_alloc)(void);
    void (*inode_free)(struct inode *inode);
    void (*sync)(struct superblock *);
};

struct filesystem_type {
    const char *name;
    struct list_head list;
    struct superblock *(*mount)(struct fsdev *, enum mount_flags);
    void (*unmount)(struct superblock *);
};

extern spinlock_t filesystem_lock;
extern struct list_head filesystem_list;

/**
 * sb_read - use superblock to read filesystem device
 * @sb: the superblock to read data
 * @pos: position (byte)
 * @buf: read buffer
 * @len: read length
 */
static inline state sb_read(struct superblock *sb, unsigned long pos, void *buf, size_t len)
{
    return fsdev_read(sb->fsdev, pos, buf, len);
}

/**
 * sb_write - use superblock to write filesystem device
 * @sb: the superblock to write data
 * @pos: position (byte)
 * @buf: write buffer
 * @len: write length
 */
static inline state sb_write(struct superblock *sb, unsigned long pos, void *buf, size_t len)
{
    return fsdev_write(sb->fsdev, pos, buf, len);
}

/* filesystem lib */
extern struct inode *fslib_inode_alloc(void);
extern void fslib_inode_free(struct inode *inode);
extern struct superblock *fslib_superblock_alloc(void);
extern void fslib_superblock_free(struct superblock *sb);

/* vfl core */
extern struct file *vfl_open(const char *name, int flags, umode_t mode);
extern ssize_t vfl_read(struct file *fp, void *buf, size_t len, loff_t *pos);
extern void __init vfl_init(void);

/* filesystem manager */
extern state filesystem_register(struct filesystem_type *fs);
extern void filesystem_unregister(struct filesystem_type *fs);

/* init */
extern state kernel_link(const char *oldname, const char *newname);
extern state kernel_symlink(const char *oldname, const char *newname);
extern state kernel_unlink(const char *filename);
extern state kernel_chdir(const char *path);
extern state kernel_chroot(const char *path);
extern state kernel_mount(const char *devn, const char *fsn, enum mount_flags flags);
extern state kernel_unmount(const char *pathname);
extern state kernel_execve(const char *file, const char *const *argv, const char *const *envp);

#endif /* _FILESYSTEM_H_ */
