/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/video.h>
#include <driver/fbcon.h>

state video_register(struct video_device *vdev)
{
    fbcon_register(vdev);

    return -ENOERR;
}

void video_unregister(struct video_device *vdev)
{

}

static state video_init(void)
{

    return -ENOERR;
}
framework_initcall(video_init);
