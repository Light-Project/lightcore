/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <font.h>
#include <driver/vt.h>
#include <driver/video.h>
#include <export.h>

struct vt_ops fbcon_ops = {

};

state fbcon_device_register(struct video_device *vdev)
{

    return -ENOERR;
}
EXPORT_SYMBOL(fbcon_device_register);

void fbcon_device_unregister(struct video_device *vdev)
{

}
EXPORT_SYMBOL(fbcon_device_unregister);
