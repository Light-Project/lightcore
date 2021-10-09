/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "vesa-display: " fmt

#include <driver/video.h>
#include <driver/video/vesa.h>
#include <driver/video/vbe.h>
#include <printk.h>

#include <asm/io.h>

struct vesa_device {
    struct video_device video;
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

static void vesa_hwblank(struct video_device *vdev, bool enable)
{
    struct vesa_device *vesa = video_to_vesa(vdev);
    vesa_vga_read(vesa, VESA_IS1_RC);
    vesa_vga_write(vesa, VESA_ATT_W, enable ? 0 : 0x20);
}

static state vesa_setmode(struct video_device *vdev)
{
    struct vesa_device *vesa = video_to_vesa(vdev);
    const struct video_mode *mode = vdev->cur_mode;

    /* select video card */
    vesa_write(vesa, VBE_DISPI_INDEX_ENABLE, 0);

    /* Set video card */
    vesa_write(vesa, VBE_DISPI_INDEX_XRES, mode->xres);
    vesa_write(vesa, VBE_DISPI_INDEX_YRES, mode->yres);
    vesa_write(vesa, VBE_DISPI_INDEX_VIRT_WIDTH, mode->xres);
    vesa_write(vesa, VBE_DISPI_INDEX_VIRT_HEIGHT, mode->yres);
    vesa_write(vesa, VBE_DISPI_INDEX_X_OFFSET, 0);
    vesa_write(vesa, VBE_DISPI_INDEX_Y_OFFSET, 0);

    /* Fixed parameters */
    vesa_write(vesa, VBE_DISPI_INDEX_BANK, 0);
    vesa_write(vesa, VBE_DISPI_INDEX_BPP, 32);

    /* Enable video card */
    vesa_write(vesa, VBE_DISPI_INDEX_ENABLE,
               VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    return -ENOERR;
}

const struct video_mode vesa_video_mode[] = {
    { .name = "480p",   .xres = 640,   .yres = 480 },
    { .name = "720p",   .xres = 1280,  .yres = 720 },
    { .name = "768p",   .xres = 1366,  .yres = 768 },
    { .name = "1080p",  .xres = 1920,  .yres = 1080 },
    { .name = "2k",     .xres = 2560,  .yres = 1440 },
    { }, /* NULL */
};

#if defined(CONFIG_VIDEO_BOCHS)
#include "bochs.c"
#endif

#if defined(CONFIG_VIDEO_BOCHS)
#include "vbox.c"
#endif

