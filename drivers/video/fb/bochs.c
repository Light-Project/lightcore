/* SPDX-License-Identifier: GPL-2.0-or-later */

#define DRIVER_NAME "bochs-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <stddef.h>
#include <string.h>
#include <mm.h>
#include <ioport.h>
#include <driver/pci.h>
#include <init/initcall.h>
#include <driver/video.h>
#include <driver/video/bochs.h>
#include <printk.h>

#include <asm/io.h>

struct bochs_device {
    void    *vram_start;
    size_t  vram_size;
    void    *mmio;
    struct  video_device device;
};
#define to_bochs_device(video_device) \
        container_of(video_device, struct bochs_device, device);

/* Default video mode */
#define def_xres    1280
#define def_yres    720
#define def_bpp     24      /* RR-GG-BB-UU */

static uint16_t bochs_hw_read(struct bochs_device *dev, uint8_t reg)
{
    uint16_t val;

    if(dev->mmio){
		int offset = 0x500 + (reg << 1);
		val = readw(dev->mmio + offset);
    } else {
        outw(VBE_DISPI_IOPORT_INDEX, reg);
        val = inw(VBE_DISPI_IOPORT_DATA);
    }
    return val;
}

static void bochs_hw_write(struct bochs_device *dev, uint8_t reg, uint32_t val)
{
    if(dev->mmio) {
		int offset = 0x500 + (reg << 1);
		writew(dev->mmio + offset, val);
    } else {
        outw(VBE_DISPI_IOPORT_INDEX, reg);
        outw(VBE_DISPI_IOPORT_DATA, val);
    }
}

/**
 * bochs_set_endian - Set video endian
 * 
 */
#ifndef CONFIG_CPU_BIG_ENDIAN
#define bochs_set_endian(a) bochs_set_little_endian(a)
static void bochs_set_little_endian(struct bochs_device *bochs)
{
    writel(bochs->mmio + 0x604, 0x1e1e1e1e);
}
#else
#define bochs_set_endian(a) bochs_set_big_endian(a)
static void bochs_set_big_endian(struct bochs_device *bochs)
{
    writel(bochs->mmio + 0x604, 0xbebebebe);
}
#endif

static state bochs_setmode(struct video_device *vdev)
{
    struct bochs_device *dev = to_bochs_device(vdev);
    
    /* select video card */
    outb(0x3c0, 0x20);
    bochs_hw_write(dev, VBE_DISPI_INDEX_ENABLE, 0);

    /* Set video card */
    bochs_hw_write(dev, VBE_DISPI_INDEX_XRES, vdev->mode.xres);
    bochs_hw_write(dev, VBE_DISPI_INDEX_YRES, vdev->mode.yres);
    bochs_hw_write(dev, VBE_DISPI_INDEX_VIRT_WIDTH, vdev->mode.vxres);
    bochs_hw_write(dev, VBE_DISPI_INDEX_VIRT_HEIGHT, vdev->mode.vyres);
    bochs_hw_write(dev, VBE_DISPI_INDEX_X_OFFSET, vdev->mode.xoffset);
    bochs_hw_write(dev, VBE_DISPI_INDEX_Y_OFFSET, vdev->mode.yoffset);

    /* Fixed parameters */
    bochs_hw_write(dev, VBE_DISPI_INDEX_BANK, 0);
    bochs_hw_write(dev, VBE_DISPI_INDEX_BPP, def_bpp);

    /* Enable video card */
    bochs_hw_write(dev, VBE_DISPI_INDEX_ENABLE, 
            VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    return -ENOERR;
}

static struct video_ops bochs_ops = {
    .set_mode = bochs_setmode,
    
};

/**
 * bochs_hw_init - Startup video card
 * 
 */
static state bochs_hw_init(struct pci_device *pdev)
{
    struct bochs_device *bochs = pci_get_devdata(pdev);
    size_t mem = 0, addr = 0, size = 0;
    uint32_t ret;

    /* Mapping configuration space */
    ret = pci_resource_type(pdev, 2);
    if (ret == RESOURCE_MMIO) {
        addr = pci_resource_start(pdev, 2);
        size = pci_resource_size(pdev, 2);
        bochs->mmio = ioremap(addr, size);
        if(!bochs->mmio)
            return -ENOMEM;
    } else if(ret == RESOURCE_PMIO) {
        if(ioport_alloc(VBE_DISPI_IOPORT_INDEX, 2))
            return -EBUSY;
    } else
        return -ENODEV;

    ret = bochs_hw_read(bochs, VBE_DISPI_INDEX_ID);
    if((ret & 0xfff0) != VBE_DISPI_ID0)
        return -ENODEV;

    /* Mapping framebuffer */
    mem = bochs_hw_read(bochs, VBE_DISPI_INDEX_VIDEO_MEMORY_64K) * 64 * 1024;

    if (!(pci_resource_type(pdev, 0) & RESOURCE_MMIO))
		return -ENODEV;
    addr = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);
    if(!addr)
        return -ENODEV;
    if(mem != size) {
		pr_err("Size mismatch: pci=%ld, bochs=%ld\n\r", size, mem);
        size = min(mem, size);
    }

    bochs->vram_start = ioremap(addr, size);
    bochs->vram_size  = size;
    if(!bochs->vram_start)
        return -ENOMEM;
    
    pr_info("Framebuffer size %ldKiB @ 0x%lx\n\r", size / 1024, addr);

    if(bochs->mmio)
        bochs_set_endian(bochs);

    return -ENOERR;
}

/**
 * bochs_hw_deinit - Shutdown video card
 * 
 */
static state bochs_hw_deinit(struct pci_device *pdev)
{
    struct bochs_device *bochs = pci_get_devdata(pdev);

    /* Deinit framebuffer */
    memset(bochs->vram_start, 0x00, bochs->vram_size);
    iounmap(bochs->vram_start);
    
    /* Deinit configuration space */
    if(bochs->mmio)
        iounmap(bochs->mmio);

    return -ENOERR;
}

static struct pci_device_id bochs_device_id[] = {
    {
        .vendor     = 0x1234,
        .device     = 0x1111,
        .subvendor  = PCI_ANY_ID,
        .subdevice  = PCI_ANY_ID,
    },
    { }, /* NULL */
};

static state bochs_probe(struct pci_device *pdev, int pdata)
{
    struct bochs_device *bochs;
    state ret;

    return -ENOERR;

    bochs = kmalloc(sizeof(*bochs), GFP_KERNEL);
    if(!bochs)
        return -ENOMEM;
    pci_set_devdata(pdev, bochs);

    ret = bochs_hw_init(pdev);
    if(ret)
        goto exit_free;

    /* Default resolution */
    bochs->device.mode.xres = def_xres;
    bochs->device.mode.yres = def_yres;
    bochs->device.mode.vxres = def_xres;
    bochs->device.mode.vyres = def_yres;
    bochs_setmode(&bochs->device);

    bochs->device.ops = &bochs_ops;
    video_register(&bochs->device);

    return -ENOERR;

exit_free:
    kfree(bochs);
    return ret;
}

static state bochs_remove(struct pci_device *pdev)
{
    return bochs_hw_deinit(pdev);
}

static struct pci_driver bochs_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = bochs_device_id,
    .probe = bochs_probe,
    .remove = bochs_remove,
};

static state bochs_init(void)
{
    return pci_driver_register(&bochs_driver);
}
driver_initcall(bochs_init);
