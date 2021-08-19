/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <font.h>
#include <logo.h>
#include <driver/tty.h>
#include <driver/fbcon.h>

static void fbcon_putc(struct tty_device *tty, int xpos, int ypos)
{

}

struct tty_ops fb_tty = {
    .putc = fbcon_putc,
};

state fbcon_register(struct video_device *vdev)
{
    // struct font *font;

    // font = font_suitable(vdev->mode.vxres, vdev->mode.vxres, 0 , 0);

    return -ENOERR;
}


void __init fb_console_init()
{
}

