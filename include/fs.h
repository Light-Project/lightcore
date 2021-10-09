/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_H_
#define _FS_H_

#include <types.h>
#include <bits.h>
#include <state.h>
#include <error.h>
#include <list.h>
#include <time.h>
#include <fsdev.h>
#include <fs/dirent.h>

extern struct list_head filesystem_list;

typedef enum fmode {
    FMODE_RDONLY    = BIT(0),
    FMODE_WRONLY    = BIT(1),
    FMODE_RW        = FMODE_RDONLY | FMODE_WRONLY,

    FMODE_LSEEK     = BIT(3),
    FMODE_PREAD     = BIT(4),
    FMODE_EXEC      = BIT(5),
} fmode_t;

enum mount_flag {
    MOUNT_RDONLY    = BIT(0),
    MOUNT_WRONLY    = BIT(1),
    MOUNT_RW        = MOUNT_RDONLY | MOUNT_WRONLY,
};

struct file {
    enum fmode fmode;
    const char *path;
    loff_t  offset;

    struct file_ops *ops;
    void *data;
};

struct inode {
    umode_t mode;
    uid_t   uid;
    gid_t   gid;

    struct timespec atime;  /* access time */
    struct timespec mtime;  /* modify time */
    struct timespec ctime;  /* change time */

    struct superblock *sb;
    struct inode_ops *ops;
    struct file_ops *fops;
    struct list_head list;  /* inode LRU list */
};

struct superblock {
    struct filesystem_type *fs_type;
    struct fsdev *fsdev;

    struct inode *root;
    struct list_head inode;

    struct superblock_ops *ops;
    struct list_head mount_list;
};

struct file_ops {
    state (*open)(struct inode *, struct file *);
    state (*close)(struct inode *, struct file *);
    state (*read)(struct file *fp, void *dest, size_t len, loff_t *off);
    state (*write)(struct file *fp, void *src, size_t len, loff_t *off);
    state (*mmap)(struct file *fp);
};

struct inode_ops {
    state (*mkdir)(struct dirent *, struct inode *, umode_t);
    state (*rmdir)(struct dirent *, struct inode *);
    struct dirent *(*lookup)(struct inode *, struct dirent *, unsigned int);
    state (*readlink)(struct dirent *, char *, int);
    state (*symlink)(struct inode *, struct dirent *, const char *);
    state (*link)(struct dirent *, struct inode *,struct dirent *);
    state (*unlink)(struct inode *, struct dirent *);
};

struct superblock_ops {
    void (*sync)(struct superblock *);
};

struct filesystem_type {
    const char *name;
    struct list_head list;
    struct superblock *(*mount)(struct fsdev *, enum mount_flag);
    state (*unmount)(struct dirent *);
};

static inline state sb_read(struct superblock *sb, unsigned long pos,
                            void *buf, size_t len)
{
    return fsdev_read(sb->fsdev, pos, buf, len);
}

static inline state sb_write(struct superblock *sb, unsigned long pos,
                             void *buf, size_t len)
{
    return fsdev_write(sb->fsdev, pos, buf, len);
}

static inline state sb_page_read(struct superblock *sb, unsigned long pos,
                            void *buf, size_t len)
{
    return fsdev_read(sb->fsdev, pos, buf, len);
}

struct file *vfl_open(const char *name, int flags, umode_t mode);
ssize_t vfl_read(struct file *fp, void *buf, size_t len, loff_t *pos);

state kernel_execve(const char *file, const char *const *argv, const char *const *envp);

#define filesystem_for_each(fs) \
    list_for_each_entry(fs, &filesystem_list, list)

state filesystem_register(struct filesystem_type *fs);
void filesystem_unregister(struct filesystem_type *fs);

void vfl_init(void);

#endif  /* _FS_H_ */
