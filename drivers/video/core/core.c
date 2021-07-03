/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <init/initcall.h>
#include <driver/video.h>
#include <driver/fbcon.h>

static void video_fixup(struct video_device *vdev)
{
    if((!vdev->mode_list.next)&&(!vdev->mode_list.prev))
        list_head_init(&vdev->mode_list);
}

state video_register(struct video_device *vdev)
{

    video_fixup(vdev);

    fbcon_register(vdev);

    return -ENOERR;
}

state fb_logo_init(struct video_device *vdev)
{

    return -ENOERR;
}

static state fbcon_init(void)
{


    return -ENOERR;
}

framework_initcall(fbcon_init);