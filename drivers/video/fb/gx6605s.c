/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/video.h>
#include <driver/video/gx6605s.h>
#include <printk.h> 

#include <asm/io.h>

struct gx6605s_device {
    struct video_device video;
    void *mmio, *region;
};

#define video_to_gdev(video_device) \
    container_of(video_device, struct gx6605s_device, video)

static uint32_t palette_buffer[2] = { 0, 0xff00ff00 };

static __always_inline uint32_t
gx6605s_readl(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static __always_inline void
gx6605s_writel(struct gx6605s_device *gdev, uint16_t reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static __always_inline void
gx6605s_region_set(struct gx6605s_device *gdev, uint16_t index, uint32_t val)
{
    writel(gdev->region + index, val);
}

static state gx6605s_setmode(struct video_device *vdev)
{
    struct gx6605s_device *gdev = video_to_gdev(vdev);
    const struct video_mode *mode = vdev->cur_mode;

    gx6605s_region_set(gdev, GX6605S_OSDR_WIDTH, mode->xres);
    gx6605s_region_set(gdev, GX6605S_OSDR_HIGHT, mode->yres);

    return -ENOERR;
}

static const struct video_mode gx6605s_video_mode[] = {
    {.name = "480p",   .xres = 640,   .yres = 480 },
    {.name = "720p",   .xres = 1280,  .yres = 720 },
    {.name = "768p",   .xres = 1366,  .yres = 768 },
    {.name = "1080p",  .xres = 1920,  .yres = 1080 },
    { }, /* NULL */
};

static struct video_ops gx6605s_ops = {
    .set_mode = gx6605s_setmode,
};

static state gx6605s_hwinit(struct gx6605s_device *gdev)
{
    uint32_t val;
    
    gdev->video.frame_buffer = kmalloc(0x800000, GFP_KERNEL);
    if (!gdev->video.frame_buffer)
        return -ENOMEM;

    /**
     * Since we only need one layer,
     * we only need to init one region 
     */
    gdev->region = kmalloc(0x1c, GFP_KERNEL);
    if (!gdev->region) {
        kfree(gdev->video.frame_buffer);
        return -ENOMEM;
    }

    val = GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN | 
         (GX6605S_OSDR_CTRL_MIX_WEIGHT & (0xff << 16));
    gx6605s_region_set(gdev, GX6605S_OSDR_CTRL, val);

    val = va_to_pa(palette_buffer);
    gx6605s_region_set(gdev, GX6605S_OSDR_CLUT_PTR, val);

    val = va_to_pa(gdev->video.frame_buffer);
    gx6605s_region_set(gdev, GX6605S_OSDR_FBADDR, val);

    val = va_to_pa(gdev->region);
    gx6605s_region_set(gdev, GX6605S_OSDR_NEXT_PTR, val);

    val = gx6605s_readl(gdev, GX6605S_OSD_CTRL);
    val &= ~GX6605S_OSD_CTRL_ANTI_FLICKER_CBCR;
    gx6605s_writel(gdev, GX6605S_OSD_CTRL, val);

    gx6605s_writel(gdev, GX6605S_OSD_VIEW_SIZE, 0x10001000);

    val = gx6605s_readl(gdev, GX6605S_OSD_CTRL);
    val |= GX6605S_OSD_CTRL_EN;
    gx6605s_writel(gdev, GX6605S_OSD_CTRL, val);

    return -ENOERR;
}

static state gx6605s_probe(struct platform_device *pdev)
{
    struct gx6605s_device *gdev;
    state ret;

    gdev = kzalloc(sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOERR;
    platform_set_devdata(pdev, gdev);

    ret = gx6605s_hwinit(gdev);
    if (ret) {
        kfree(gdev);
        return ret;
    }

    gdev->video.mode_table = gx6605s_video_mode;
    gdev->video.cur_mode = &gx6605s_video_mode[1];
    gx6605s_setmode(&gdev->video);

    gdev->video.device = &pdev->device;
    gdev->video.ops = &gx6605s_ops;
    video_register(&gdev->video);

    return -ENOERR;
}

static state gx6605s_remove(struct platform_device *pdev)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);

    gx6605s_writel(gdev, GX6605S_OSD_CTRL, 0);

    kfree(gdev);
    return -ENOERR;
}

static struct dt_device_id gx6605s_ids[] = {
    { .compatible = "csky,gx6605s-vpu" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_ids,
    .probe = gx6605s_probe,
    .remove = gx6605s_remove,
};

static state gx6605s_fb_init(void)
{
    return platform_driver_register(&gx6605s_driver);
}
driver_initcall(gx6605s_fb_init);