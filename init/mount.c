/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "mount"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <init.h>
#include <filesystem.h>
#include <printk.h>

static state __init auto_mount(const char *devn, enum mount_flags flags)
{
    struct filesystem_type *fs;
    state retval = -ENOENT;

    spin_lock(&filesystem_lock);
    list_for_each_entry(fs, &filesystem_list, list) {
        const char *name = fs->name;

        pr_info("Try mount '%s' on '%s'\n", name, devn);
        retval = kernel_mount(devn, name, flags);

        switch (retval) {
            case -ENOERR:
                goto exit;

            case -EINVAL:
                continue;

            default:
                break;
        }

        pr_crit("Can't mount root device %s\n", devn);
    }

exit:
    spin_unlock(&filesystem_lock);
    return -ENOENT;
}

#ifdef CONFIG_BLOCK
static state __init mount_block_root(void)
{
    return auto_mount("/dev/root", MOUNT_RW);
}
#endif

#ifdef CONFIG_ROMDISK
static state __init mount_romdisk_root(void)
{
    return -ENOERR;
}
#endif

state __init mount_rootfs(void)
{
    state ret;

#ifdef CONFIG_BLOCK
    ret = mount_block_root();
    if (!ret)
        return -ENOERR;
    pr_warn("Unable to mount rootfs on block (%d)\n", ret);
#endif

#ifdef CONFIG_ROMDISK
    ret = mount_romdisk_root();
    if (!ret)
        return -ENOERR;
    pr_warn("Unable to mount rootfs on romdisk (%d)\n", ret);
#endif

    return ret;
}
