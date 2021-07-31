/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <init/initcall.h>
#include <driver/video.h>
#include <driver/fbcon.h>

state video_register(struct video_device *vdev)
{
    fbcon_register(vdev);

    return -ENOERR;
}

static state fbcon_init(void)
{

    return -ENOERR;
}
framework_initcall(fbcon_init);
