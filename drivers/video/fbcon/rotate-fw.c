/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/fbcon.h>

static void rotate_fw_bmove(struct fbcon_device *fdev, unsigned int xsrc, unsigned int ysrc,
                            unsigned int xdest, unsigned int ydest, unsigned int width, unsigned int height)
{
    struct video_copyarray copyarray = {
        .xsrc   = xsrc   * fdev->font->width,
        .ysrc   = ysrc   * fdev->font->height,
        .xdest  = xdest  * fdev->font->width,
        .ydest  = ydest  * fdev->font->height,
        .width  = width  * fdev->font->width,
        .height = height * fdev->font->height,
    };

    video_copyarray(fdev->vdev, &copyarray);
}

static void rotate_fw_clear(struct fbcon_device *fdev, unsigned int xpos, unsigned int ypos,
                            unsigned int width, unsigned int height)
{
    struct video_fillrect fillrect = {
        .xpos   = xpos   * fdev->font->width,
        .ypos   = ypos   * fdev->font->height,
        .width  = width  * fdev->font->width,
        .height = height * fdev->font->height,
        .rop    = VIDEO_ROP_COPY,
    };

    video_fillrect(fdev->vdev, &fillrect);
}

static struct fbcon_rotate rotate_fw_ops = {
    .rotate = 0,
    .bmove = rotate_fw_bmove,
    .clear = rotate_fw_clear,
};

static state rotate_fw_init(void)
{
    return fbcon_rotate_register(&rotate_fw_ops);
}
framework_initcall(rotate_fw_init);
