/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/video.h>
#include <export.h>

LIST_HEAD(video_list);
SPIN_LOCK(video_lock);

state video_register(struct video_device *vdev)
{
    if (!vdev->dev || !vdev->ops)
        return -EINVAL;

    spin_lock(&video_lock);
    list_add(&video_list, &vdev->list);
    spin_unlock(&video_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(video_register);

void video_unregister(struct video_device *vdev)
{
    spin_lock(&video_lock);
    list_del(&vdev->list);
    spin_unlock(&video_lock);
}
EXPORT_SYMBOL(video_unregister);
