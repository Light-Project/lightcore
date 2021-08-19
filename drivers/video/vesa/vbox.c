/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <init/initcall.h>
#include <driver/pci.h>

static struct video_ops vbox_ops = {
    .set_mode = vesa_setmode,
};

static state vbox_hw_init(struct pci_device *pdev)
{

    return -ENOERR;
}

static state vbox_probe(struct pci_device *pdev, int pdata)
{
    struct vesa_device *vesa;
    state ret;

    if(vesa_ready)
        return -ENOERR;

    vesa = kmalloc(sizeof(*vesa), GFP_KERNEL);
    if(!vesa)
        return -ENOMEM;
    pci_set_devdata(pdev, vesa);

    ret = vbox_hw_init(pdev);
    if (ret) {
        kfree(vesa);
        return ret;
    }

    /* Default resolution */
    vesa->video.mode_table = vesa_video_mode;
    vesa->video.cur_mode = &vesa_video_mode[1];
    vesa_setmode(&vesa->video);

    vesa->video.device = &pdev->dev;
    vesa->video.ops = &vbox_ops;
    video_register(&vesa->video);

    return -ENOERR;
}

static state vbox_remove(struct pci_device *pdev)
{

    return -ENOERR;
}

static struct pci_device_id vbox_id_table[] = {
    { PCI_DEVICE(0x80ee, 0xbeef) },
    { }, /* NULL */
};

static struct pci_driver vbox_driver = {
    .driver = {
        .name = "vbox-video",
    },
    .id_table = vbox_id_table,
    .probe = vbox_probe,
    .remove = vbox_remove,
};

static state vbox_init(void)
{
    return pci_driver_register(&vbox_driver);
}
driver_initcall(vbox_init);
