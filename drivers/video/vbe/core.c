/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "vbe"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kernel.h>
#include <driver/video.h>
#include <driver/video/vga.h>
#include <driver/video/bochs.h>
#include <printk.h>
#include <asm/io.h>

struct vesa_device {
    struct video_device video;
    spinlock_t lock;
    void *mmio, *vgabase;
};

#define video_to_vesa(video_device) \
    container_of(video_device, struct vesa_device, video)

static __always_inline uint8_t
vesa_vga_read(struct vesa_device *vesa, uint16_t reg)
{
    if (reg < 0x3c0 || reg > 0x3df)
        return 0xff;

    if (vesa->vgabase)
        return readb(vesa->vgabase + reg - 0x3c0);

    return inb(reg);
}

static __always_inline void
vesa_vga_write(struct vesa_device *vesa, uint16_t reg, uint8_t val)
{
    if (reg < 0x3c0 || reg > 0x3df)
        return;

    if (vesa->vgabase)
        return writeb(vesa->vgabase + reg - 0x3c0, val);

    outb(reg, val);
}

static __always_inline uint16_t
vesa_read(struct vesa_device *vesa, uint16_t reg)
{
    if (vesa->mmio)
        return readw(vesa->mmio + (reg << 1));

    outw(VBE_DISPI_IOPORT_INDEX, reg);
    return inw(VBE_DISPI_IOPORT_DATA);
}

static __always_inline void
vesa_write(struct vesa_device *vesa, uint16_t reg, uint16_t val)
{
    if (vesa->mmio)
        return writew(vesa->mmio + (reg << 1), val);

    outw(VBE_DISPI_IOPORT_INDEX, reg);
    outw(VBE_DISPI_IOPORT_DATA, val);
}

static state vesa_hwblank(struct video_device *vdev, enum video_blank mode)
{
    struct vesa_device *vesa = video_to_vesa(vdev);

    spin_lock(&vesa->lock);
    vesa_vga_read(vesa, VGA_IS1_RC);

    switch (mode) {
        case VIDEO_BLANK_DISABLE:
            vesa_vga_write(vesa, VGA_ATT_W, 0);
            break;
        case VIDEO_BLANK_NORMAL:
        case VIDEO_BLANK_POWERDOWN:
        case VIDEO_BLANK_VSYNC_SUSPEND:
        case VIDEO_BLANK_HSYNC_SUSPEND:
            vesa_vga_write(vesa, VGA_ATT_W, 0x20);
            break;
        default:
            return -EINVAL;
    }

    spin_unlock(&vesa->lock);
    return -ENOERR;
}

static state vesa_panoff(struct video_device *vdev)
{
    struct vesa_device *vesa = video_to_vesa(vdev);
    const struct video_screen *info = &vdev->screen;

    spin_lock(&vesa->lock);

    /* set video card */
    vesa_write(vesa, VBE_DISPI_INDEX_X_OFFSET, info->xpos);
    vesa_write(vesa, VBE_DISPI_INDEX_Y_OFFSET, info->ypos);

    spin_unlock(&vesa->lock);
    return -ENOERR;
}

static state vesa_checkmode(struct video_device *vdev, struct video_screen *info)
{
    const struct video_mode *vmode;
    unsigned int line_size;

    /* support RGB565 */
    if (info->bpp == 16 &&
       (info->blue.length !=     5 || info->green.length !=     6 ||
        info->red.length  !=     5 || info->alpha.length !=     0 ||
        info->blue.offset !=     0 || info->green.offset !=     5 ||
        info->red.offset  !=    11 || info->alpha.offset !=     0 ||
        info->blue.msbr   != false || info->green.msbr   != false ||
        info->red.msbr    != false || info->alpha.msbr   != false )) {
        dev_debug(vdev->dev, "RGB565 parameter error\n");
        return -EINVAL;
    }

    /* support ARGB8888 */
    if (info->bpp == 32 &&
       (info->blue.length !=     8 || info->green.length !=     8 ||
        info->red.length  !=     8 || info->alpha.length !=     8 ||
        info->blue.offset !=     0 || info->green.offset !=     8 ||
        info->red.offset  !=    16 || info->alpha.offset !=    24 ||
        info->blue.msbr   != false || info->green.msbr   != false ||
        info->red.msbr    != false || info->alpha.msbr   != false )) {
        dev_debug(vdev->dev, "ARGB8888 parameter error\n");
        return -EINVAL;
    }

    /* Out of range */
    if (info->xres + info->xpos > info->xres_virtual ||
        info->yres + info->ypos > info->yres_virtual ) {
        dev_debug(vdev->dev, "panning out-of-range\n");
        return -EINVAL;
    }

    /* Screen size */
    if (info->xres < 64 || info->yres < 64) {
        dev_debug(vdev->dev, "screen too small\n");
        return -EINVAL;
    }

    /* Rotation is not supported */
    if (info->rotate) {
        dev_debug(vdev->dev, "rotate is not supported\n");
        return -EINVAL;
    }

    /* Memory limit */
    line_size = info->xres_virtual * info->bpp / 8;
    if (line_size * info->yres_virtual > vdev->frame_size) {
        dev_debug(vdev->dev, "no enough framebuffer\n");
        return -EINVAL;
    }

    vmode = video_mode_best(vdev, info);
    if (!vmode)
        return -EINVAL;

    return -ENOERR;
}

static state vesa_setmode(struct video_device *vdev)
{
    struct vesa_device *vesa = video_to_vesa(vdev);
    struct video_screen *info = &vdev->screen;
    const struct video_mode *vmode;

    vmode = video_mode_best(vdev, info);
    if (!vmode)
        return -EINVAL;

    vdev->state.xpanstep = !!info->xres_virtual - info->xres;
    vdev->state.ypanstep = !!info->yres_virtual - info->yres;
    vdev->state.line_size = vmode->xres * info->bpp / 8;
    vdev->cur_mode = vmode;

    spin_lock(&vesa->lock);

    /* select video card */
    vesa_write(vesa, VBE_DISPI_INDEX_ENABLE, 0);

    /* set video card */
    vesa_write(vesa, VBE_DISPI_INDEX_XRES, vmode->xres);
    vesa_write(vesa, VBE_DISPI_INDEX_YRES, vmode->yres);
    vesa_write(vesa, VBE_DISPI_INDEX_BPP, info->bpp);
    vesa_write(vesa, VBE_DISPI_INDEX_VIRT_WIDTH, info->xres_virtual);
    vesa_write(vesa, VBE_DISPI_INDEX_VIRT_HEIGHT, info->yres_virtual);
    vesa_write(vesa, VBE_DISPI_INDEX_X_OFFSET, 0);
    vesa_write(vesa, VBE_DISPI_INDEX_Y_OFFSET, 0);

    /* enable video card */
    vesa_write(vesa, VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    spin_unlock(&vesa->lock);
    return -ENOERR;
}

#if defined(CONFIG_VBE_BOCHS)
# include "bochs.c"
#endif

#if defined(CONFIG_VBE_VBOX)
# include "vbox.c"
#endif
