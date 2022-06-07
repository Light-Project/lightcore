/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/pci.h>

static struct video_ops vbox_ops = {
    .blank = vesa_hwblank,
    .panoff = vesa_panoff,
    .checkmode = vesa_checkmode,
    .setmode = vesa_setmode,
    .imageblit = video_imageblit,
    .fillrect = video_fillrect,
};

static state vbox_hw_init(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    resource_size_t addr, size;

    addr = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);
    if (!addr)
        return -ENODEV;

    vesa->video.frame_size = size;
    vesa->video.framebuffer = dev_ioremap(&pdev->dev, addr, size);
    if (!vesa->video.framebuffer)
        return -ENOMEM;

    pr_info("Framebuffer size %ldKiB @ 0x%lx\n", size / 1024, addr);
    return -ENOERR;
}

static state vbox_probe(struct pci_device *pdev, const void *pdata)
{
    struct vesa_device *vesa;
    state ret;

    vesa = dev_kzalloc(&pdev->dev, sizeof(*vesa), GFP_KERNEL);
    if(!vesa)
        return -ENOMEM;
    pci_set_devdata(pdev, vesa);

    ret = vbox_hw_init(pdev);
    if (ret)
        return ret;

    vesa->video.dev = &pdev->dev;
    vesa->video.ops = &vbox_ops;
    return video_register(&vesa->video);
}

static void vbox_remove(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    video_unregister(&vesa->video);
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
