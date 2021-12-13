/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <initcall.h>
#include <driver/pci.h>

#ifndef CONFIG_CPU_BIG_ENDIAN
#define bochs_set_endian(a) bochs_set_little_endian(a)
static void bochs_set_little_endian(struct vesa_device *vesa)
{
    writel(vesa->mmio + 0x104, 0x1e1e1e1e);
}
#else
#define bochs_set_endian(a) bochs_set_big_endian(a)
static void bochs_set_big_endian(struct vesa_device *bochs)
{
    writel(vesa->mmio + 0x104, 0xbebebebe);
}
#endif

static state bochs_setmode(struct video_device *vdev)
{
    vesa_hwblank(vdev, false);
    return vesa_setmode(vdev);
}

static struct video_ops bochs_ops = {
    .set_mode = bochs_setmode,
};

static state bochs_hw_init(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    resource_size_t addr, size, ioaddr, iosize;
    size_t mem, ret;
    void *base;

    /* Mapping configuration space */
    ret = pci_resource_type(pdev, 2);
    if (ret == RESOURCE_MMIO) {
        ioaddr = pci_resource_start(pdev, 2);
        iosize = pci_resource_size(pdev, 2);
        base = dev_ioremap(&pdev->dev, ioaddr, iosize);
        if (!base)
            return -ENOMEM;
        vesa->vgabase = base + 0x400;
        vesa->mmio = base + 0x500;
    } else if (ret != RESOURCE_PMIO)
        return -ENODEV;

    ret = vesa_read(vesa, VBE_DISPI_INDEX_ID);
    if ((ret & 0xfff0) != VBE_DISPI_ID0)
        return -ENODEV;

    mem = vesa_read(vesa, VBE_DISPI_INDEX_VIDEO_MEMORY_64K) * 64 * 1024;

    if (pci_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;
    addr = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);
    if (!addr)
        return -ENODEV;
    if (mem != size) {
        dev_info(&pdev->dev, "size mismatch: pci=%ld, bochs=%ld\n", size, mem);
        size = min(mem, size);
    }

    vesa->video.frame_buffer = dev_ioremap(&pdev->dev, addr, size);
    vesa->video.frame_size = size;
    if (!vesa->video.frame_buffer)
        return -ENOMEM;

    dev_info(&pdev->dev, "framebuffer size 0x%#lx @ 0x%lx\n", size, addr);

    if (vesa->mmio && pdev->revision >= 2) {
        ret = readl(vesa->mmio + 0x100);
        if (8 <= ret && ret < iosize) {
            dev_info(&pdev->dev, "found ext regs, size %ld\n", ret);
            bochs_set_endian(vesa);
        }
    }

    return -ENOERR;
}

static state bochs_probe(struct pci_device *pdev, const void *pdata)
{
    struct vesa_device *vesa;
    state ret;

    vesa = dev_kzalloc(&pdev->dev, sizeof(*vesa), GFP_KERNEL);
    if(!vesa)
        return -ENOMEM;
    pci_set_devdata(pdev, vesa);

    ret = bochs_hw_init(pdev);
    if (ret)
        return ret;

    vesa->video.device = &pdev->dev;
    vesa->video.ops = &bochs_ops;
    return video_register(&vesa->video);
}

static void bochs_remove(struct pci_device *pdev)
{
    struct vesa_device *vesa = pci_get_devdata(pdev);
    video_unregister(&vesa->video);
}

static struct pci_device_id bochs_id_table[] = {
    { PCI_DEVICE(0x1234, 0x1111) }, /* bochs */
    { }, /* NULL */
};

static struct pci_driver bochs_driver = {
    .driver = {
        .name = "bochs-video",
    },
    .id_table = bochs_id_table,
    .probe = bochs_probe,
    .remove = bochs_remove,
};

static state bochs_init(void)
{
    return pci_driver_register(&bochs_driver);
}
driver_initcall(bochs_init);
