/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <driver/video.h>
#include <export.h>

#define GENERIC_VIDEO_OPS(operation, rtype, errval, args, ...)      \
rtype video_##operation(struct video_device *vdev, ##__VA_ARGS__)   \
{                                                                   \
    struct video_ops *ops = vdev->ops;                              \
                                                                    \
    if (!ops->operation)                                            \
        return errval;                                              \
                                                                    \
    return ops->operation args;                                     \
}                                                                   \
EXPORT_SYMBOL(video_##operation)

GENERIC_VIDEO_OPS(enable, state, -EOPNOTSUPP, (vdev));
GENERIC_VIDEO_OPS(disable, state, -EOPNOTSUPP, (vdev));
GENERIC_VIDEO_OPS(sync, state, -EOPNOTSUPP, (vdev));
GENERIC_VIDEO_OPS(read, state, -EOPNOTSUPP, (vdev, buff, count, ppos), void *buff, size_t count, loff_t *ppos);
GENERIC_VIDEO_OPS(write, state, -EOPNOTSUPP, (vdev, buff, count, ppos), void *buff, size_t count, loff_t *ppos);
GENERIC_VIDEO_OPS(ioctl, state, -EOPNOTSUPP, (vdev, cmd, arg), unsigned int cmd, unsigned long arg);
GENERIC_VIDEO_OPS(mmap, state, -EOPNOTSUPP, (vdev, vmem), struct vmem_area *vmem);
GENERIC_VIDEO_OPS(blank, state, -EOPNOTSUPP, (vdev, mode), enum video_blank mode);
GENERIC_VIDEO_OPS(panoff, state, -EOPNOTSUPP, (vdev));
GENERIC_VIDEO_OPS(setmode, state, -EOPNOTSUPP, (vdev));
GENERIC_VIDEO_OPS(checkmode, state, -EOPNOTSUPP, (vdev, info), struct video_screen *info);
GENERIC_VIDEO_OPS(imageblit, state, -EOPNOTSUPP, (vdev, image), const struct video_image *image);
GENERIC_VIDEO_OPS(fillrect, state, -EOPNOTSUPP, (vdev, fillrect), const struct video_fillrect *fillrect);
GENERIC_VIDEO_OPS(copyarray, state, -EOPNOTSUPP, (vdev, copyarray), const struct video_copyarray *copyarray);
