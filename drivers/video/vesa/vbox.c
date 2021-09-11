/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <initcall.h>
#include <driver/pci.h>

static struct video_ops vbox_ops = {
    .set_mode = vesa_setmode,
};

static state vbox_hw_init(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    resource_size_t addr, size;

    addr = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);
    if (!addr)
        return -ENODEV;

    vesa->video.frame_buffer = dev_ioremap(&pdev->dev, addr, size);
    vesa->video.frame_size = size;

    pr_info("Framebuffer size %dKiB @ 0x%x\n", size / 1024, addr);

    return -ENOERR;
}

#include <string.h>

static state vbox_probe(struct pci_device *pdev, int pdata)
{
    struct vesa_device *vesa;
    state ret;

    vesa = dev_kmalloc(&pdev->dev, sizeof(*vesa), GFP_KERNEL);
    if(!vesa)
        return -ENOMEM;
    pci_set_devdata(pdev, vesa);

    ret = vbox_hw_init(pdev);
    if (ret)
        return ret;

    vesa->video.mode_table = vesa_video_mode;
    vesa->video.cur_mode = &vesa_video_mode[3];
    vesa_setmode(&vesa->video);
    memset(vesa->video.frame_buffer, 0xff, vesa->video.frame_size);

    vesa->video.device = &pdev->dev;
    vesa->video.ops = &vbox_ops;
    video_register(&vesa->video);

    return -ENOERR;
}

static void vbox_remove(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    video_unregister(&vesa->video);
}

static struct pci_device_id vbox_id_table[] = {
    { PCI_DEVICE(0x80ee, 0xbeef) }, /* vbox */
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
