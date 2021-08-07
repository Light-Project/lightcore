/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_H_
#define _DRIVER_VIDEO_H_

#include <types.h>
#include <state.h>
#include <lightcore/video.h>
#include <mm.h>

struct video_device;

struct video_mode {
    const char *name;
    uint32_t xres;
    uint32_t yres;
    uint32_t refresh;
};

struct video_ops {
    state (*open)(struct video_device *);
    state (*release)(struct video_device *);

    state (*read)(struct video_device *);
    state (*write)(struct video_device *);
    state (*mmap)(struct video_device *, void *);
    state (*imageblit)(struct video_device *, const struct video_image *);

    state (*set_mode)(struct video_device *);
};

struct video_device {
    struct device *device;  /* the parent device */

    const struct video_mode *cur_mode;
    const struct video_mode *mode_table;

    void  *frame_buffer;
    size_t frame_size;

    struct video_ops *ops;
};

state video_register(struct video_device *);

#endif
