/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "blk-part"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <driver/block.h>
#include <driver/block/partition.h>
#include <export.h>
#include <printk.h>

static LIST_HEAD(partition_list);
static SPIN_LOCK(partition_lock);

static struct partition_type *partition_find(const char *name)
{
    struct partition_type *part;

    list_for_each_entry(part, &partition_list, list)
        if (!strcmp(part->name, name))
            return part;

    return NULL;
}

state partition_scan(struct block_device *bdev)
{
    struct partition_type *part;
    state ret;

    spin_lock(&partition_lock);
    list_for_each_entry(part, &partition_list, list) {
        ret = part->match(bdev);
        if (ret == -ENODATA)
            continue;
        return ret;
    }
    spin_unlock(&partition_lock);

    return -ENODATA;
}

state partition_register(struct partition_type *part)
{
    if (!part->name || !part->match) {
        pr_err("illegal register (%p)", part);
        return -EINVAL;
    }

    spin_lock(&partition_lock);
    if (partition_find(part->name)) {
        spin_unlock(&partition_lock);
        pr_err("partition %s already registered\n", part->name);
        return -EALREADY;
    }

    list_add(&partition_list, &part->list);
    spin_unlock(&partition_lock);

    pr_debug("register partition '%s'\n", part->name);
    return -ENOERR;
}
EXPORT_SYMBOL(partition_register);

void partition_unregister(struct partition_type *part)
{
    spin_lock(&partition_lock);
    list_del(&part->list);
    spin_unlock(&partition_lock);
    pr_debug("unregister partition '%s'\n", part->name);
}
EXPORT_SYMBOL(partition_unregister);
