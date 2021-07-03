/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_H_
#define _DRIVER_VIDEO_H_

#include <types.h>
#include <state.h>
#include <lightcore/video.h>
#include <mm.h>

struct video_device;

struct video_mode {
    uint32_t xres;      /* visible resolution */
    uint32_t yres;
    uint32_t vxres;     /* virtual resolution */
    uint32_t vyres;
    uint32_t xoffset;   /* virtual visible offset */
    uint32_t yoffset;

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

    struct list_head mode_list; /* Mode list */
    struct video_mode mode;     /* Current video mode */

    struct video_ops *ops;
};

state video_register(struct video_device *);

#endif
