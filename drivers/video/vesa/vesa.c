/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "vesa-display: " fmt

#include <driver/video.h>
#include <driver/video/vesa.h>
#include <driver/video/bochs.h>
#include <printk.h>

#include <asm/io.h>

struct vesa_device {
    struct  video_device video;
    void    *mmio;
};

#define video_to_vesa(video_device) \
    container_of(video_device, struct vesa_device, video)

static inline uint16_t vesa_read(struct vesa_device *vesa,
                                 uint8_t reg)
{
    uint16_t val;

    if (vesa->mmio)
        val = readw(vesa->mmio + (reg << 1));
    else {
        outw(VBE_DISPI_IOPORT_INDEX, reg);
        val = inw(VBE_DISPI_IOPORT_DATA);
    }
    return val;
}

static inline void vesa_write(struct vesa_device *vesa,
                              uint8_t reg, uint32_t val)
{
    if (vesa->mmio)
        writew(vesa->mmio + (reg << 1), val);
    else {
        outw(VBE_DISPI_IOPORT_INDEX, reg);
        outw(VBE_DISPI_IOPORT_DATA, val);
    }
}

static inline state vesa_setmode(struct video_device *vdev)
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
    {.name = "480p",   .xres = 640,   .yres = 480 },
    {.name = "720p",   .xres = 1280,  .yres = 720 },
    {.name = "768p",   .xres = 1366,  .yres = 768 },
    {.name = "1080p",  .xres = 1920,  .yres = 1080 },
    {.name = "2k",     .xres = 2560,  .yres = 1440 },
    { }, /* NULL */
};

#if defined(CONFIG_VIDEO_BOCHS)
#include "bochs.c"
#endif

#if defined(CONFIG_VIDEO_BOCHS)
#include "vbox.c"
#endif

