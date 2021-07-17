/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FS_H_
#define _FS_H_

#include <types.h>
#include <bits.h>
#include <state.h>
#include <list.h>

typedef enum fmode {
    FMODE_RDONLY    = BIT(0),
    FMODE_WRONLY    = BIT(1),
    FMODE_RW        = FMODE_RDONLY | FMODE_WRONLY,

    FMODE_LSEEK     = BIT(3),
    FMODE_PREAD     = BIT(4),
    FMODE_EXEC      = BIT(5),
} fmode_t;

struct file;
struct inode;
struct super_block;

struct file_ops {
    loff_t (*llseek)(struct file *file, loff_t, int);
    state (*read)(struct file *file, char *, size_t, loff_t *);
    state (*write)(struct file *file, const char *, size_t, loff_t *);

};

struct inode_ops {
    // struct dentry *(*lookup)(struct inode *,struct dentry *, unsigned int);
	// int (*readlink) (struct dentry *, char *, int);

	// int (*create) (struct user_namespace *, struct inode *,struct dentry *,
	// 	       umode_t, bool);
	// int (*link) (struct dentry *,struct inode *,struct dentry *);
	// int (*unlink) (struct inode *,struct dentry *);
	// int (*symlink) (struct user_namespace *, struct inode *,struct dentry *,
	// 		const char *);
	// int (*mkdir) (struct user_namespace *, struct inode *,struct dentry *,
	// 	      umode_t);
	// int (*rmdir) (struct inode *,struct dentry *);

};

struct sb_ops {
    void (*sync)(struct super_block *);
};

struct file {


};

struct inode {


};

struct super_block {
    struct sb_ops *ops;
};

struct fs_type {
    char *name;
    list_t  list;
};

state kernel_execve(const char *file, const char *const *argv, const char *const *envp);

state filesystem_register(struct fs_type *fs);

#endif  /* _FS_H_ */
