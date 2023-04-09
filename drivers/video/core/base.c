/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <framedraw.h>
#include <driver/video.h>
#include <export.h>

state video_cfbimageblit(struct video_device *vdev, const struct video_image *image)
{
    struct framedraw_info framedraw = {
        .bpp = vdev->screen.bpp,
        .line_size = vdev->state.line_size,
        .framebuffer = vdev->framebuffer,
        .frameswab = video_need_convert(vdev),
    };

    video_sync(vdev);
    imageblit(&framedraw, image);

    return -ENOERR;
}
EXPORT_SYMBOL(video_cfbimageblit);

state video_cfbfillrect(struct video_device *vdev, const struct video_fillrect *rect)
{
    struct framedraw_info framedraw = {
        .bpp = vdev->screen.bpp,
        .line_size = vdev->state.line_size,
        .framebuffer = vdev->framebuffer,
        .frameswab = video_need_convert(vdev),
    };

    video_sync(vdev);
    fillrect(&framedraw, rect);

    return -ENOERR;
}
EXPORT_SYMBOL(video_cfbfillrect);

state video_cfbcopyarray(struct video_device *vdev, const struct video_copyarray *array)
{
    struct framedraw_info framedraw = {
        .bpp = vdev->screen.bpp,
        .line_size = vdev->state.line_size,
        .framebuffer = vdev->framebuffer,
        .frameswab = video_need_convert(vdev),
    };

    video_sync(vdev);
    copyarray(&framedraw, array);

    return -ENOERR;
}
EXPORT_SYMBOL(video_cfbcopyarray);
