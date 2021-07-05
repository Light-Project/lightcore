/* SPDX-License-Identifier: GPL-2.0-or-later */

#define DRIVER_NAME "vesa-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <stddef.h>
#include <string.h>
#include <mm.h>
#include <ioport.h>
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/video.h>
#include <driver/video/vesa.h>
#include <printk.h> 

#include <asm/io.h>


static struct video_ops bochs_ops = {
    .set_mode = bochs_setmode,
    
};

static state vesa_probe(struct platform_device *pdev)
{

    return -ENOERR;
}

static state vesa_remove(struct platform_device *pdev)
{

    return -ENOERR;
}

static struct platform_driver bochs_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = vesa_probe,
    .remove = vesa_remove,
};

static state vesa_init(void)
{
    return platform_driver_register(&bochs_driver);
}

driver_initcall(vesa_init);
