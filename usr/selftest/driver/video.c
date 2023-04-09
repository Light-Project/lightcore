/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>
#include <delay.h>
#include <font.h>
#include <driver/video.h>

#define FILLRECT_XRES 80
#define FILLRECT_YRES 80
#define PANOFF_GRANUL 10

static void video_test_copyarray(struct video_device *vdev)
{
    struct video_copyarray copyarray = {};
    unsigned int count, limit;

    limit = min(vdev->screen.xres, vdev->screen.yres);
    copyarray.width = vdev->screen.xres;
    copyarray.height = vdev->screen.yres;
    copyarray.xdest = copyarray.ydest = 0;
    copyarray.xsrc = copyarray.ysrc = PANOFF_GRANUL;

    for (count = 0; count <= limit; count += PANOFF_GRANUL) {
        video_copyarray(vdev, &copyarray);
        msleep(10);
    }
}

static void video_test_panoff(struct video_device *vdev)
{
    unsigned int count;

    for (count = 0; count <= vdev->screen.xres; count += PANOFF_GRANUL) {
        vdev->screen.xpos = count;
        video_panoff(vdev);
        msleep(10);
    }

    vdev->screen.xpos = 0;
    for (count = 0; count <= vdev->screen.yres; count += PANOFF_GRANUL) {
        vdev->screen.ypos = count;
        video_panoff(vdev);
        msleep(10);
    }

    vdev->screen.ypos = 0;
    video_panoff(vdev);
}

static void video_test_imageblit(struct video_device *vdev)
{
    struct video_image image = {};
    unsigned int xpos, ypos, count = 0;
    struct font *font;

    font = font_suitable(vdev->screen.xres, vdev->screen.yres, -1, -1);
    if (!font)
        return;

    image.width = font->width;
    image.height = font->height;
    image.fgcolor = UINT16_MAX;
    image.depth = 1;

    for (ypos = 0; ypos + image.height <= vdev->screen.yres; ypos += image.height) {
        for (xpos = 0; xpos + image.width <= vdev->screen.xres; xpos += image.width) {
            image.xpos = xpos;
            image.ypos = ypos;
            image.data = font_index(font, count++);
            video_imageblit(vdev, &image);
        }
    }
}

static void video_test_fillrect(struct video_device *vdev)
{
    struct video_fillrect fillrect = {};
    unsigned int xpos, ypos, count;

    fillrect.width = FILLRECT_XRES;
    fillrect.height = FILLRECT_YRES;
    fillrect.rop = VIDEO_ROP_XOR;
    fillrect.color = UINT16_MAX;

    for (count = 0; count < 2; ++count) {
        for (ypos = 0; ypos + FILLRECT_YRES <= vdev->screen.yres; ypos += FILLRECT_YRES) {
            for (xpos = 0; xpos + FILLRECT_YRES <= vdev->screen.xres; xpos += FILLRECT_YRES) {
                fillrect.xpos = xpos;
                fillrect.ypos = ypos;
                video_fillrect(vdev, &fillrect);
                msleep(10);
            }
        }
    }
}

static void video_test_setmode(struct video_device *vdev)
{
    struct video_modelist *vmlist;

    vmlist = list_first_entry_or_null(&vdev->modes, struct video_modelist, list);
    if (!vmlist)
        return;

    vdev->screen.xres = vmlist->mode->xres;
    vdev->screen.yres = vmlist->mode->yres;
    vdev->screen.bpp = BITS_PER_U16;

    video_setmode(vdev);
}

static state video_testing(struct kshell_context *ctx, void *pdata)
{
    struct video_device *vdev;

    spin_lock(&video_lock);
    list_for_each_entry(vdev, &video_list, list) {
        video_enable(vdev);
        video_test_setmode(vdev);
        video_test_fillrect(vdev);
        video_test_imageblit(vdev);
        video_test_panoff(vdev);
        video_test_copyarray(vdev);
    }
    spin_unlock(&video_lock);

    return -ENOERR;
}

static struct selftest_command video_command = {
    .group = "driver",
    .name = "video",
    .desc = "video unit test",
    .testing = video_testing,
};

static state selftest_video_init(void)
{
    return selftest_register(&video_command);
}
kshell_initcall(selftest_video_init);
