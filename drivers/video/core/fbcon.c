/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <font.h>
#include <driver/vt.h>
#include <driver/video.h>
#include <export.h>

// static void fbcon_putc(struct tty_device *tty, int xpos, int ypos)
// {

// }

struct vt_ops fbcon_ops = {
};

state video_console_register(struct video_device *vdev)
{
    return -ENOERR;
}
EXPORT_SYMBOL(video_console_register);

void __init fbcon_init(void)
{

}
