/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm.h>
#include <ioport.h>
#include <initcall.h>
#include <driver/pci.h>

static state vesa_probe(struct pci_device *pdev, const void *pdata)
{
    struct vesa_device *vesa;
    state ret;

    if(vesa_ready)
        return -ENOERR;

    vesa = kzalloc(sizeof(*vesa), GFP_KERNEL);
    if(!vesa)
        return -ENOMEM;
    pci_set_devdata(pdev, vesa);

    /* Default resolution */
    vesa->video.mode_table = vesa_video_mode;
    vesa->video.cur_mode = &vesa_video_mode[1];
    vesa_setmode(&vesa->video);

    vesa->video.device = &pdev->dev;
    vesa->video.ops = &vbox_ops;
    video_register(&vesa->video);

    return -ENOERR;
}

static state vesa_remove(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);

    kfree(vesa);

    return -ENOERR;
}

static struct pci_device_id vesa_id_table[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_DISPLAY_VGA << 8, ~0) },
    { }, /* NULL */
};

static struct pci_driver vbox_driver = {
    .driver = {
        .name = "vesa-video",
    },
    .id_table = vesa_id_table,
    .probe = vesa_probe,
    .remove = vesa_remove,
};

static state vbox_init(void)
{
    return pci_driver_register(&vbox_driver);
}
driver_initcall(vbox_init);
