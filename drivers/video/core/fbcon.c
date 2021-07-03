/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * frame buffer based console driver 
 */

#include <mm.h>
#include <driver/fbcon.h>
#include <console.h>
#include <font.h> 

static void fbcon_write(struct console *con, 
                         const char *str, unsigned len)
{
    // struct video_device *vdev = con->data;


}

struct console fb_con = {
    .write = fbcon_write,
};


state fbcon_setup()
{


    return -ENOERR;
}

state fbcon_register(struct video_device *vdev)
{
    // struct font *font;

    // font = font_suitable(vdev->mode.vxres, vdev->mode.vxres, 0 , 0);

    console_register(&fb_con);

    return -ENOERR;
}


void __init fb_console_init()
{
    console_lock();

    console_unlock();
}

