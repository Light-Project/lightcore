/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_FBCON_H_
#define _DRIVER_FBCON_H_

#include <font.h>
#include <driver/video.h>
#include <driver/vt.h>

struct fbcon_device {
    struct vt_device vt;
    struct list_head list;
    struct fbcon_ops *ops;
    struct font *font;
    struct video_device *vdev;
    unsigned int rotate;
};

struct fbcon_rotate {
    unsigned int rotate;
    struct list_head list;
    void (*cursor)(struct fbcon_device *fdev, unsigned int mode, uint32_t fg, uint32_t bg);
    void (*putcs)(struct fbcon_device *fdev, const char *str, unsigned int length, unsigned int xpos, unsigned int yops, uint32_t fg, uint32_t bg);
    void (*clear)(struct fbcon_device *fdev, unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height);
    void (*bmove)(struct fbcon_device *fdev, unsigned int xsrc, unsigned int ysrc, unsigned int xdest, unsigned int ydest, unsigned int width, unsigned int height);
};

extern state fbcon_device_register(struct video_device *vdev);
extern void fbcon_device_unregister(struct video_device *vdev);

extern state fbcon_rotate_set(struct fbcon_device *fdev);
extern state fbcon_rotate_register(struct fbcon_rotate *ops);
extern void fbcon_rotate_unregister(struct fbcon_rotate *ops);

#endif /* _DRIVER_FBCON_H_ */
