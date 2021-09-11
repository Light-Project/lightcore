/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/video.h>
#include <driver/video/gx6605s.h>
#include <printk.h>

#include <asm/io.h>
#include <asm/cache.h>

struct gx6605s_device {
    struct video_device video;
    void *mmio, *region;
    int byteseq;
};

#define video_to_gdev(video_device) \
    container_of(video_device, struct gx6605s_device, video)

enum gx6605s_byte_order {
    GX6605S_DCBA_HGFE   = 0,
    GX6605S_EFGH_ABCD   = 1,
    GX6605S_HGFE_DCBA   = 2,
    GX6605S_ABCD_EFGH   = 3,
    GX6605S_CDAB_GHEF   = 4,
    GX6605S_FEHG_BADC   = 5,
    GX6605S_GHEF_CDAB   = 6,
    GX6605S_BADC_FEHG   = 7,
};

enum gx6605s_color_fmt {
    GX6605S_COLOR_CLUT1             = 0,
    GX6605S_COLOR_CLUT2             = 1,
    GX6605S_COLOR_CLUT4             = 2,
    GX6605S_COLOR_CLUT8             = 3,
    GX6605S_COLOR_RGBA4444          = 4,
    GX6605S_COLOR_RGBA5551          = 5,
    GX6605S_COLOR_RGB565            = 6,
    GX6605S_COLOR_RGBA8888          = 7,
    GX6605S_COLOR_RGB888            = 8,
    GX6605S_COLOR_BGR888            = 9,

    GX6605S_COLOR_ARGB4444          = 10,
    GX6605S_COLOR_ARGB1555          = 11,
    GX6605S_COLOR_ARGB8888          = 12,

    GX6605S_COLOR_YCBCR422          = 13,
    GX6605S_COLOR_YCBCRA6442        = 14,
    GX6605S_COLOR_YCBCR420          = 15,

    GX6605S_COLOR_YCBCR420_Y_UV     = 16,
    GX6605S_COLOR_YCBCR420_Y_U_V    = 17,
    GX6605S_COLOR_YCBCR420_Y        = 18,
    GX6605S_COLOR_YCBCR420_U        = 19,
    GX6605S_COLOR_YCBCR420_V        = 20,
    GX6605S_COLOR_YCBCR420_UV       = 21,

    GX6605S_COLOR_YCBCR422_Y_UV     = 22,
    GX6605S_COLOR_YCBCR422_Y_U_V    = 23,
    GX6605S_COLOR_YCBCR422_Y        = 24,
    GX6605S_COLOR_YCBCR422_U        = 25,
    GX6605S_COLOR_YCBCR422_V        = 26,
    GX6605S_COLOR_YCBCR422_UV       = 27,

    GX6605S_COLOR_YCBCR444          = 28,
    GX6605S_COLOR_YCBCR444_Y_UV     = 29,
    GX6605S_COLOR_YCBCR444_Y_U_V    = 30,
    GX6605S_COLOR_YCBCR444_Y        = 31,
    GX6605S_COLOR_YCBCR444_U        = 32,
    GX6605S_COLOR_YCBCR444_V        = 33,
    GX6605S_COLOR_YCBCR444_UV       = 34,

    GX6605S_COLOR_YCBCR400          = 35,
    GX6605S_COLOR_A8                = 36,
    GX6605S_COLOR_ABGR4444          = 37,
    GX6605S_COLOR_ABGR1555          = 39,
    GX6605S_COLOR_Y8                = 40,
    GX6605S_COLOR_UV16              = 41,
    GX6605S_COLOR_YCBCR422v         = 42,
    GX6605S_COLOR_YCBCR422h         = 43,
};

static uint32_t palette_buffer[2] = {0, 0xff00ff00};

static inline uint32_t
gx6605s_readl(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static inline void
gx6605s_writel(struct gx6605s_device *gdev, uint16_t reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static inline void
gx6605s_mask(struct gx6605s_device *gdev, uint16_t reg, uint32_t clr, uint32_t set)
{
    uint32_t val = gx6605s_readl(gdev, reg);
    gx6605s_writel(gdev, reg, (val & ~clr) | set);
}

static inline uint32_t
gx6605s_region_get(struct gx6605s_device *gdev, uint16_t index)
{
    uint32_t val = readl(gdev->region + index);
    if (gdev->byteseq == GX6605S_DCBA_HGFE)
        val = constant_swab32(val);
    else if (gdev->byteseq == GX6605S_CDAB_GHEF)
        val = constant_swahw32(val);
    return val;
}

static inline void
gx6605s_region_set(struct gx6605s_device *gdev, uint16_t index, uint32_t val)
{
    if (gdev->byteseq == GX6605S_DCBA_HGFE)
        val = constant_swab32(val);
    else if (gdev->byteseq == GX6605S_CDAB_GHEF)
        val = constant_swahw32(val);
    writel(gdev->region + index, val);
}

static inline void
gx6605s_region_mask(struct gx6605s_device *gdev, uint16_t index, uint32_t clr, uint32_t set)
{
    uint32_t val = gx6605s_region_get(gdev, index);
    gx6605s_region_set(gdev, index, (val & ~clr) | set);
}

static void gx6605s_enable(struct gx6605s_device *gdev, bool enable)
{
    while((gx6605s_readl(gdev, GX6605S_OSD_CTRL) & GX6605S_OSD_CTRL_EN) != enable)
        gx6605s_mask(gdev, GX6605S_OSD_CTRL, GX6605S_OSD_CTRL_EN, enable);
}

static void gx6605s_endian_set(struct gx6605s_device *gdev, enum gx6605s_byte_order byte_seq)
{
    gdev->byteseq = byte_seq;

    if (byte_seq >> 2)
        gx6605s_mask(gdev, GX6605S_SYS_PARA, 0, GX6605S_SYS_PARA_BYTESEQ_HIGH);
    else
        gx6605s_mask(gdev, GX6605S_SYS_PARA, GX6605S_SYS_PARA_BYTESEQ_HIGH, 0);

    gx6605s_mask(gdev, GX6605S_SYS_PARA, GX6605S_SYS_PARA_BYTESEQ_LOW | 1,
                (byte_seq << 20) & GX6605S_SYS_PARA_BYTESEQ_LOW);
    gx6605s_mask(gdev, GX6605S_SYS_PARA, 0x07<<12, (byte_seq & 0x07) << 12);
}

static void gx6605s_alpha_set(struct gx6605s_device *gdev, uint32_t alpha)
{
    gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_MIX_WEIGHT,
        GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN | (GX6605S_OSDR_CTRL_MIX_WEIGHT & (0xff << 16)));
    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, GX6605S_OSDR_ALPHA_RATIO_EN, 0);
}

static void gx6605s_format_set(struct gx6605s_device *gdev, enum gx6605s_color_fmt format)
{
    if (format <= GX6605S_COLOR_CLUT8)
        gx6605s_mask(gdev, GX6605S_OSD_CTRL, GX6605S_OSD_CTRL_ZOOM_MODE_EN_IPS, 0);
    else
        gx6605s_mask(gdev, GX6605S_OSD_CTRL, 0, GX6605S_OSD_CTRL_ZOOM_MODE_EN_IPS);

    if ((GX6605S_COLOR_RGBA8888 <= format) && (format <= GX6605S_COLOR_BGR888)) {
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_COLOR_MODE);
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_TRUE_COLOR_MODE,
                           (format - GX6605S_COLOR_RGBA8888) << 24);
    } else {
        if (format == GX6605S_COLOR_ARGB4444 || format == GX6605S_COLOR_ARGB1555 ||
            format == GX6605S_COLOR_RGBA8888)
            gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_ARGB_CONVERT);
        else
            gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_ARGB_CONVERT, 0);

        switch (format) {
            case GX6605S_COLOR_ARGB4444:
                format = GX6605S_COLOR_RGBA4444;
                break;
            case GX6605S_COLOR_ARGB1555:
                format = GX6605S_COLOR_RGBA5551;
                break;
            case GX6605S_COLOR_ARGB8888:
                format = GX6605S_COLOR_RGBA8888;
                break;
            default:
                break;
        }

        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_COLOR_MODE,
                           (format << 10) & GX6605S_OSDR_CTRL_COLOR_MODE);
    }
}

static state gx6605s_setmode(struct video_device *vdev)
{

    return -ENOERR;
}

static const struct video_mode gx6605s_video_mode[] = {
    {.name = "720p",   .xres = 1280,  .yres = 720 },
    { }, /* NULL */
};

static struct video_ops gx6605s_ops = {
    .set_mode = gx6605s_setmode,
};

#define def_xres    1280
#define def_yres    720

static state gx6605s_hwinit(struct platform_device *pdev)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);
    unsigned int byte_seq, request_block, bpp = 16;
    resource_size_t start, size;
    int val;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    gdev->mmio = ioremap(start, size);
    if (!gdev->mmio)
        return -ENOMEM;

    gdev->video.frame_buffer = kzalloc(0x200000, GFP_KERNEL);
    if (!gdev->video.frame_buffer)
        return -ENOMEM;

    gdev->region = kzalloc(page_align(0x1c), GFP_KERNEL);
    if (!gdev->region) {
        iounmap(gdev->mmio);
        kfree(gdev->video.frame_buffer);
        return -ENOMEM;
    }

    if(bpp == 32)
        byte_seq = GX6605S_ABCD_EFGH;
    if(bpp == 16)
        byte_seq = GX6605S_CDAB_GHEF;
    if(bpp <= 8)
        byte_seq = GX6605S_DCBA_HGFE;
    gx6605s_endian_set(gdev, byte_seq);

    if (bpp == 1)
        request_block = def_xres / 4 / 128 * 128 / 8;
    else
        request_block = def_xres * (bpp >> 3) / 4 / 128 * 128;
    request_block = clamp(request_block, 128U, 896U);

    gx6605s_mask(gdev, GX6605S_BUFF_CTRL2, GX6605S_BUFF_CTRL2_REQ_LEN,
                 request_block & GX6605S_BUFF_CTRL2_REQ_LEN);

    gx6605s_region_mask(gdev, GX6605S_OSDR_WIDTH, GX6605S_OSDR_WIDTH_LEFT, 0);
    gx6605s_region_mask(gdev, GX6605S_OSDR_WIDTH, GX6605S_OSDR_WIDTH_RIGHT, (def_xres - 1) << 16);
    gx6605s_region_mask(gdev, GX6605S_OSDR_HIGHT, GX6605S_OSDR_HIGHT_TOP, 0);
    gx6605s_region_mask(gdev, GX6605S_OSDR_HIGHT, GX6605S_OSDR_HIGHT_BOTTOM, (def_yres - 1) << 16);
    gx6605s_writel(gdev, GX6605S_OSD_POSITION, 0);

    gx6605s_alpha_set(gdev, 0xff);

    if (bpp == 1) {
        gx6605s_format_set(gdev, GX6605S_COLOR_CLUT1);
        gx6605s_region_set(gdev, GX6605S_OSDR_CLUT_PTR, va_to_pa(palette_buffer));
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_CLUT_LENGTH, 0);
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_CLUT_UPDATA_EN);
    } else {
        gx6605s_format_set(gdev, GX6605S_COLOR_RGB565);
    }

    gx6605s_region_set(gdev, GX6605S_OSDR_FBADDR, va_to_pa(gdev->video.frame_buffer));
    gx6605s_region_set(gdev, GX6605S_OSDR_NEXT_PTR, va_to_pa(gdev->region));

    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, 0, GX6605S_OSDR_LIST_END);
    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, GX6605S_OSDR_BASELINE, def_xres);

    gx6605s_mask(gdev, GX6605S_OSD_CTRL, GX6605S_OSD_CTRL_ANTI_FLICKER_CBCR, 0);

    gx6605s_writel(gdev, GX6605S_OSD_VIEW_SIZE, (def_yres << 16) | def_xres);
    gx6605s_writel(gdev, GX6605S_OSD_ZOOM, 0x10001000);

    gx6605s_writel(gdev, GX6605S_OSD_FIRST_HEAD_PTR, va_to_pa(gdev->region));

    gx6605s_enable(gdev, true);
    dcache_writeback_all();

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

    ret = gx6605s_hwinit(pdev);
    if (ret) {
        kfree(gdev);
        return ret;
    }

    gdev->video.mode_table = gx6605s_video_mode;
    gdev->video.cur_mode = &gx6605s_video_mode[0];
    gdev->video.device = &pdev->device;
    gdev->video.ops = &gx6605s_ops;
    return video_register(&gdev->video);
}

static void gx6605s_remove(struct platform_device *pdev)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);

    gx6605s_enable(gdev, false);

    iounmap(gdev->mmio);
    kfree(gdev->mmio);
    kfree(gdev->video.frame_buffer);
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