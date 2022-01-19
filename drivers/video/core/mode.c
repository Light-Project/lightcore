/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "video-mode"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kmalloc.h>
#include <driver/video.h>
#include <printk.h>
#include <export.h>

static const struct video_mode video_mode_std[] = {{
        .name = "640x400@70hz", .refresh = 70,
        .xres = 640, .yres = 400, .pixclk = 39721,
        .upper_margin = 39, .lower_margin = 9,
        .left_margin = 40, .right_margin = 24,
        .hsync_len = 96, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x480@60", .refresh = 60,
        .xres = 640, .yres = 480, .pixclk = 39721,
        .upper_margin = 32, .lower_margin = 11,
        .left_margin = 40, .right_margin = 24,
        .hsync_len = 96, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "800x600@56", .refresh = 56,
        .xres = 800, .yres = 600, .pixclk = 27777,
        .upper_margin = 22, .lower_margin = 1,
        .left_margin = 128, .right_margin = 24,
        .hsync_len = 72, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@87", .refresh = 87,
        .xres = 1024, .yres = 768, .pixclk = 22271,
        .upper_margin = 33, .lower_margin = 8,
        .left_margin = 56, .right_margin = 24,
        .hsync_len = 160, .vsync_len = 8,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x400@85", .refresh = 85,
        .xres = 640, .yres = 400, .pixclk = 31746,
        .upper_margin = 41, .lower_margin = 1,
        .left_margin = 96, .right_margin = 32,
        .hsync_len = 64, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x480@72", .refresh = 72,
        .xres = 640, .yres = 480, .pixclk = 31746,
        .upper_margin = 30, .lower_margin = 8,
        .left_margin = 144, .right_margin = 40,
        .hsync_len = 40, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x480@75", .refresh = 75,
        .xres = 640, .yres = 480, .pixclk = 31746,
        .upper_margin = 16, .lower_margin = 1,
        .left_margin = 120, .right_margin = 16,
        .hsync_len = 64, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "800x600@60", .refresh = 60,
        .xres = 800, .yres = 600, .pixclk = 25000,
        .upper_margin = 23, .lower_margin = 1,
        .left_margin = 88, .right_margin = 40,
        .hsync_len = 128, .vsync_len = 4,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x480@85", .refresh = 85,
        .xres = 640, .yres = 480, .pixclk = 27777,
        .upper_margin = 25, .lower_margin = 1,
        .left_margin = 80, .right_margin = 56,
        .hsync_len = 56, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@89", .refresh = 89,
        .xres = 1152, .yres = 864, .pixclk = 15384,
        .upper_margin = 110, .lower_margin = 1,
        .left_margin = 96, .right_margin = 16,
        .hsync_len = 216, .vsync_len = 10,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "800x600@72", .refresh = 72,
        .xres = 800, .yres = 600, .pixclk = 20000,
        .upper_margin = 23, .lower_margin = 37,
        .left_margin = 64, .right_margin = 56,
        .hsync_len = 120, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@60", .refresh = 60,
        .xres = 1024, .yres = 768, .pixclk = 15384,
        .upper_margin = 29, .lower_margin = 3,
        .left_margin = 168, .right_margin = 8,
        .hsync_len = 144, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "640x480@100", .refresh = 100,
        .xres = 640, .yres = 480, .pixclk = 21834,
        .upper_margin = 36, .lower_margin = 8,
        .left_margin = 96, .right_margin = 32,
        .hsync_len = 96, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@60", .refresh = 60,
        .xres = 1152, .yres = 864, .pixclk = 11123,
        .upper_margin = 16, .lower_margin = 4,
        .left_margin = 208, .right_margin = 64,
        .hsync_len = 256, .vsync_len = 8,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "800x600@85", .refresh = 85,
        .xres = 800, .yres = 600, .pixclk = 16460,
        .upper_margin = 36, .lower_margin = 16,
        .left_margin = 160, .right_margin = 64,
        .hsync_len = 64, .vsync_len = 5,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@70", .refresh = 70,
        .xres = 1024, .yres = 768, .pixclk = 13333,
        .upper_margin = 29, .lower_margin = 3,
        .left_margin = 144, .right_margin = 24,
        .hsync_len = 136, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@87", .refresh = 87,
        .xres = 1280, .yres = 1024, .pixclk = 12500,
        .upper_margin = 128, .lower_margin = 1,
        .left_margin = 56, .right_margin = 16,
        .hsync_len = 216, .vsync_len = 12,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "800x600@100", .refresh = 100,
        .xres = 800, .yres = 600, .pixclk = 14357,
        .upper_margin = 30, .lower_margin = 4,
        .left_margin = 160, .right_margin = 64,
        .hsync_len = 64, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@76", .refresh = 76,
        .xres = 1024, .yres = 768, .pixclk = 11764,
        .upper_margin = 36, .lower_margin = 16,
        .left_margin = 208, .right_margin = 8,
        .hsync_len = 120, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@70", .refresh = 70,
        .xres = 1152, .yres = 864, .pixclk = 10869,
        .upper_margin = 20, .lower_margin = 1,
        .left_margin = 106, .right_margin = 56,
        .hsync_len = 160, .vsync_len = 10,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@61", .refresh = 61,
        .xres = 1280, .yres = 1024, .pixclk = 9090,
        .upper_margin = 26, .lower_margin = 1,
        .left_margin = 200, .right_margin = 48,
        .hsync_len = 184, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1400x1050@60", .refresh = 60,
        .xres = 1400, .yres = 1050, .pixclk = 9259,
        .upper_margin = 13, .lower_margin = 1,
        .left_margin = 136, .right_margin = 40,
        .hsync_len = 112, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1400x1050@75", .refresh = 75,
        .xres = 1400, .yres = 1050, .pixclk = 7190,
        .upper_margin = 23, .lower_margin = 10,
        .left_margin = 120, .right_margin = 56,
        .hsync_len = 112, .vsync_len = 13,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1400x1050@60", .refresh = 60,
        .xres = 1400, .yres = 1050, .pixclk = 9259,
        .upper_margin = 12, .lower_margin = 0,
        .left_margin = 128, .right_margin = 40,
        .hsync_len = 112, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@85", .refresh = 85,
        .xres = 1024, .yres = 768, .pixclk = 10111,
        .upper_margin = 34, .lower_margin = 14,
        .left_margin = 192, .right_margin = 32,
        .hsync_len = 160, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@78", .refresh = 78,
        .xres = 1152, .yres = 864, .pixclk = 9090,
        .upper_margin = 32, .lower_margin = 0,
        .left_margin = 228, .right_margin = 88,
        .hsync_len = 84, .vsync_len = 12,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@70", .refresh = 70,
        .xres = 1280, .yres = 1024, .pixclk = 7905,
        .upper_margin = 28, .lower_margin = 8,
        .left_margin = 224, .right_margin = 32,
        .hsync_len = 160, .vsync_len = 8,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1600x1200@60", .refresh = 60,
        .xres = 1600, .yres = 1200, .pixclk = 6172,
        .upper_margin = 46, .lower_margin = 1,
        .left_margin = 304, .right_margin = 64,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@84", .refresh = 84,
        .xres = 1152, .yres = 864, .pixclk = 7407,
        .upper_margin = 32, .lower_margin = 0,
        .left_margin = 184, .right_margin = 312,
        .hsync_len = 128, .vsync_len = 12,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@74", .refresh = 74,
        .xres = 1280, .yres = 1024, .pixclk = 7407,
        .upper_margin = 34, .lower_margin = 3,
        .left_margin = 256, .right_margin = 32,
        .hsync_len = 144, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1024x768@100", .refresh = 100,
        .xres = 1024, .yres = 768, .pixclk = 8658,
        .upper_margin = 21, .lower_margin = 3,
        .left_margin = 192, .right_margin = 32,
        .hsync_len = 192, .vsync_len = 10,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@76", .refresh = 76,
        .xres = 1280, .yres = 1024, .pixclk = 7407,
        .upper_margin = 34, .lower_margin = 3,
        .left_margin = 248, .right_margin = 32,
        .hsync_len = 104, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1600x1200@70", .refresh = 70,
        .xres = 1600, .yres = 1200, .pixclk = 5291,
        .upper_margin = 46, .lower_margin = 1,
        .left_margin = 304, .right_margin = 64,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x864@100", .refresh = 100,
        .xres = 1152, .yres = 864, .pixclk = 7264,
        .upper_margin = 17, .lower_margin = 2,
        .left_margin = 224, .right_margin = 32,
        .hsync_len = 128, .vsync_len = 19,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@85", .refresh = 85,
        .xres = 1280, .yres = 1024, .pixclk = 6349,
        .upper_margin = 44, .lower_margin = 1,
        .left_margin = 224, .right_margin = 64,
        .hsync_len = 160, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1600x1200@75", .refresh = 75,
        .xres = 1600, .yres = 1200, .pixclk = 4938,
        .upper_margin = 46, .lower_margin = 1,
        .left_margin = 304, .right_margin = 64,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1680x1050@60", .refresh = 60,
        .xres = 1680, .yres = 1050, .pixclk = 6848,
        .upper_margin = 30, .lower_margin = 3,
        .left_margin = 280, .right_margin = 104,
        .hsync_len = 176, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1600x1200@85", .refresh = 85,
        .xres = 1600, .yres = 1200, .pixclk = 4545,
        .upper_margin = 37, .lower_margin = 4,
        .left_margin = 272, .right_margin = 16,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x1024@100", .refresh = 100,
        .xres = 1280, .yres = 1024, .pixclk = 5502,
        .upper_margin = 26, .lower_margin = 7,
        .left_margin = 256, .right_margin = 32,
        .hsync_len = 128, .vsync_len = 15,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1800x1440@64", .refresh = 64,
        .xres = 1800, .yres = 1440, .pixclk = 4347,
        .upper_margin = 46, .lower_margin = 1,
        .left_margin = 304, .right_margin = 96,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1800x1440@70", .refresh = 70,
        .xres = 1800, .yres = 1440, .pixclk = 4000,
        .upper_margin = 46, .lower_margin = 1,
        .left_margin = 304, .right_margin = 96,
        .hsync_len = 192, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "512x384@78", .refresh = 78,
        .xres = 512, .yres = 384, .pixclk = 49603,
        .upper_margin = 16, .lower_margin = 1,
        .left_margin = 48, .right_margin = 16,
        .hsync_len = 64, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "512x384@85", .refresh = 85,
        .xres = 512, .yres = 384, .pixclk = 45454,
        .upper_margin = 16, .lower_margin = 1,
        .left_margin = 48, .right_margin = 16,
        .hsync_len = 64, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "320x200@70", .refresh = 70,
        .xres = 320, .yres = 200, .pixclk = 79440,
        .upper_margin = 20, .lower_margin = 4,
        .left_margin = 16, .right_margin = 16,
        .hsync_len = 48, .vsync_len = 1,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "320x240@60", .refresh = 60,
        .xres = 320, .yres = 240, .pixclk = 79440,
        .upper_margin = 16, .lower_margin = 5,
        .left_margin = 16, .right_margin = 16,
        .hsync_len = 48, .vsync_len = 1,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "320x240@72", .refresh = 72,
        .xres = 320, .yres = 240, .pixclk = 63492,
        .upper_margin = 16, .lower_margin = 4,
        .left_margin = 16, .right_margin = 16,
        .hsync_len = 48, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "400x300@56", .refresh = 56,
        .xres = 400, .yres = 300, .pixclk = 55555,
        .upper_margin = 10, .lower_margin = 1,
        .left_margin = 64, .right_margin = 16,
        .hsync_len = 32, .vsync_len = 1,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "400x300@60", .refresh = 60,
        .xres = 400, .yres = 300, .pixclk = 50000,
        .upper_margin = 11, .lower_margin = 1,
        .left_margin = 48, .right_margin = 16,
        .hsync_len = 64, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "400x300@72", .refresh = 72,
        .xres = 400, .yres = 300, .pixclk = 40000,
        .upper_margin = 11, .lower_margin = 19,
        .left_margin = 32, .right_margin = 24,
        .hsync_len = 64, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "480x300@56", .refresh = 56,
        .xres = 480, .yres = 300, .pixclk = 46176,
        .upper_margin = 10, .lower_margin = 1,
        .left_margin = 80, .right_margin = 16,
        .hsync_len = 40, .vsync_len = 1,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "480x300@60", .refresh = 60,
        .xres = 480, .yres = 300, .pixclk = 41858,
        .upper_margin = 11, .lower_margin = 1,
        .left_margin = 56, .right_margin = 16,
        .hsync_len = 80, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "480x300@63", .refresh = 63,
        .xres = 480, .yres = 300, .pixclk = 40000,
        .upper_margin = 11, .lower_margin = 1,
        .left_margin = 56, .right_margin = 16,
        .hsync_len = 80, .vsync_len = 2,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "480x300@72", .refresh = 72,
        .xres = 480, .yres = 300, .pixclk = 33386,
        .upper_margin = 11, .lower_margin = 19,
        .left_margin = 40, .right_margin = 24,
        .hsync_len = 80, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_DOUBLE,
    },{
        .name = "1920x1200@60", .refresh = 60,
        .xres = 1920, .yres = 1200, .pixclk = 5177,
        .upper_margin = 1, .lower_margin = 38,
        .left_margin = 128, .right_margin = 336,
        .hsync_len = 208, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1152x768@60", .refresh = 60,
        .xres = 1152, .yres = 768, .pixclk = 14047,
        .upper_margin = 29, .lower_margin = 3,
        .left_margin = 158, .right_margin = 26,
        .hsync_len = 136, .vsync_len = 6,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1366x768@60", .refresh = 60,
        .xres = 1366, .yres = 768, .pixclk = 13806,
        .upper_margin = 14, .lower_margin = 3,
        .left_margin = 120, .right_margin = 10,
        .hsync_len = 32, .vsync_len = 5,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "1280x800@60", .refresh = 60,
        .xres = 1280, .yres = 800, .pixclk = 12048,
        .upper_margin = 24, .lower_margin = 1,
        .left_margin = 200, .right_margin = 64,
        .hsync_len = 136, .vsync_len = 3,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },{
        .name = "720x576@50", .refresh = 50,
        .xres = 720, .yres = 576, .pixclk = 74074,
        .upper_margin = 39, .lower_margin = 5,
        .left_margin = 64, .right_margin = 16,
        .hsync_len = 64, .vsync_len = 5,
        .sync = 0, .vmode = VIDEO_VMODE_INTERLACED,
    },{
        .name = "800x520@50", .refresh = 50,
        .xres = 800, .yres = 520, .pixclk = 58823,
        .upper_margin = 72, .lower_margin = 28,
        .left_margin = 144, .right_margin = 64,
        .hsync_len = 80, .vsync_len = 5,
        .sync = 0, .vmode = VIDEO_VMODE_INTERLACED,
    },{
        .name = "864x480@60", .refresh = 60,
        .xres = 864, .yres = 480, .pixclk = 27777,
        .upper_margin = 1, .lower_margin = 1,
        .left_margin = 1, .right_margin = 1,
        .hsync_len = 0, .vsync_len = 0,
        .sync = 0, .vmode = VIDEO_VMODE_PROGRESSIVE,
    },
    { }, /* NULL */
};

state video_modelist_create(struct list_head *head, const struct video_mode *mode, unsigned int num)
{
    struct video_modelist *modelist;
    unsigned int index;

    if (!mode) {
        mode = video_mode_std;
        num = ARRAY_SIZE(video_mode_std);
    }

    modelist = kzalloc(sizeof(*modelist) * num, GFP_KERNEL);
    if (!modelist)
        return -ENOMEM;

    list_head_init(head);

    for (index = 0; index < num; ++index) {
        modelist[index].mode = &mode[index];
        list_add_prev(head, &modelist[index].list);
    }

    return -ENOERR;
}
EXPORT_SYMBOL(video_modelist_create);

void video_modelist_destroy(struct list_head *head)
{
    struct video_modelist *modelist, *next;

	list_for_each_entry_safe(modelist, next, head, list) {
		list_del(&modelist->list);
		kfree(modelist);
	}
}
EXPORT_SYMBOL(video_modelist_destroy);

static unsigned int video_mode_score(const struct video_mode *mode, struct video_screen *info)
{
    unsigned int upper_margin, lower_margin;
    unsigned int left_margin, right_margin;
    unsigned int xres, yres;
    long xdiff, ydiff;

    /* We always want better, isn't it ;) */
    if (mode->xres < info->xres || mode->vsync_len < info->vsync_len ||
        mode->yres < info->yres || mode->hsync_len < info->hsync_len ||
        mode->pixclk < info->pixclk)
        return 0;

    /* minimum resolution */
    xres = max(mode->xres, 1);
    yres = max(mode->yres, 1);

    /* minimum margins */
    upper_margin = max(mode->upper_margin, info->upper_margin);
    lower_margin = max(mode->lower_margin, info->lower_margin);
    left_margin  = max(mode->left_margin,  info->left_margin);
    right_margin = max(mode->right_margin, info->right_margin);

    xdiff = (mode->left_margin + mode->xres + mode->right_margin) <
            (left_margin + xres + right_margin);
    if (xdiff < 0)
        return 0;

    ydiff = (mode->upper_margin + mode->yres + mode->lower_margin) -
            (upper_margin + yres + lower_margin);
    if (ydiff < 0)
        return 0;

    if (xdiff == 0 && ydiff == 0)
        return UINT_MAX;

    return UINT_MAX - ((mode->xres - xres) * (mode->yres - yres));
}

/**
 * video_mode_best - find best video mode and fixup info
 * @vdev: video device to find mode
 * @info: info to matching and fixup
 */
const struct video_mode *video_mode_best(struct video_device *vdev, struct video_screen *info)
{
    unsigned int score, best_score = 0;
    const struct video_mode *walk, *best = NULL;
    struct video_modelist *modelist;

    dev_debug(vdev->device, "wanted %u [%u] %u x %u [%u] %u\n",
        info->left_margin, info->xres, info->right_margin,
        info->upper_margin, info->ypos, info->lower_margin);

    list_for_each_entry(modelist, &vdev->modes, list) {
        walk = modelist->mode;
        score = video_mode_score(walk, info);
        if (score > best_score) {
            best_score = score;
            best = walk;
        }
    }

    if (!best) {
        dev_debug(vdev->device, "no suitable mode found\n");
        return NULL;
    }

    /* fixed fields */
    info->pixclk = best->pixclk;
    info->hsync_len = best->hsync_len;
    info->vsync_len = best->vsync_len;

    /* minimum virtual resolution */
    if (info->xres_virtual < best->xres)
        info->xres_virtual = best->xres;
    if (info->yres_virtual < best->yres)
        info->yres_virtual = best->yres;

    /* minimum margins */
    if (info->upper_margin < best->upper_margin)
        info->upper_margin = best->upper_margin;
    if (info->lower_margin < best->lower_margin)
        info->lower_margin = best->lower_margin;
    if (info->left_margin < best->left_margin)
        info->left_margin = best->left_margin;
    if (info->right_margin < best->right_margin)
        info->right_margin = best->right_margin;

    dev_debug(vdev->device, "best mode %u [%u] %u x %u [%u] %u\n",
        info->left_margin, info->xres, info->right_margin,
        info->upper_margin, info->ypos, info->lower_margin);

    return best;
}
EXPORT_SYMBOL(video_mode_best);
