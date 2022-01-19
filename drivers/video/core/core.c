/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <logo.h>
#include <initcall.h>
#include <driver/video.h>
#include <export.h>

state video_register(struct video_device *vdev)
{
    return -ENOERR;
}
EXPORT_SYMBOL(video_register);

void video_unregister(struct video_device *vdev)
{
}
EXPORT_SYMBOL(video_unregister);

static state video_init(void)
{
    return -ENOERR;
}
framework_initcall(video_init);
