/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fs.h>
#include <system/syscall.h>

static inline void sync_one(struct super_block *sb)
{
    sb->ops->sync(sb);
}

/**
 * sys_sync - sync all super
 */
long sys_sync(void)
{

    return 0;
} 
