/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/fbcon.h>
#include <export.h>

static LIST_HEAD(rotate_list);
static SPIN_LOCK(rotate_lock);

state fbcon_rotate_set(struct fbcon_device *fdev)
{
    struct fbcon_rotate *walk, *find = NULL;

    spin_lock(&rotate_lock);
    list_for_each_entry(walk, &rotate_list, list) {
        if (fdev->rotate == walk->rotate) {
            find = walk;
            break;
        }
    }
    spin_unlock(&rotate_lock);

    fdev->rotate = find;
    return find ? -ENOERR : -EINVAL;
}
EXPORT_SYMBOL(fbcon_rotate_set);

state fbcon_rotate_register(struct fbcon_rotate *rotate)
{
    if (!rotate->cursor || !rotate->putcs ||
        !rotate->clear  || !rotate->bmove)
        return -EINVAL;

    spin_lock(&rotate_lock);
    list_add(&rotate_list, &rotate->list);
    spin_unlock(&rotate_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(fbcon_rotate_register);

void fbcon_rotate_unregister(struct fbcon_rotate *rotate)
{
    spin_lock(&rotate_lock);
    list_del(&rotate->list);
    spin_unlock(&rotate_lock);
}
EXPORT_SYMBOL(fbcon_rotate_unregister);
