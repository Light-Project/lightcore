/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <driver/video.h>
#include <export.h>

state video_blank(struct video_device *vdev, enum video_blank mode)
{
    struct video_ops *ops = vdev->ops;

    if (mode > VIDEO_BLANK_HSYNC_SUSPEND)
        return -EINVAL;

    if (!ops->blank)
        return -EOPNOTSUPP;

    return ops->blank(vdev, mode);
}
EXPORT_SYMBOL(video_blank);