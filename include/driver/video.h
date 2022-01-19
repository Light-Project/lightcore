/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_H_
#define _DRIVER_VIDEO_H_

#include <device.h>
#include <lightcore/video.h>

struct vm_area;

/**
 * video_mode - videocard supportive modes
 * @name: mode name
 * @xres: visible horizontal resolution
 * @yres: visible vertical resolution
 * @pixclk: pixel clock in pico seconds
 * @upper_margin: time from horizontal sync to picture
 * @lower_margin: time from picture to horizontal sync
 * @left_margin: time from vertical sync to picture
 * @right_margin: time from picture to vertical sync
 * @hsync_length: length of horizontal sync
 * @vsync_length: length of vertical sync
 */
struct video_mode {
    const char *name;
    unsigned int refresh;
    unsigned int xres;
    unsigned int yres;
    unsigned int pixclk;
    unsigned int upper_margin;
    unsigned int lower_margin;
    unsigned int left_margin;
    unsigned int right_margin;
    unsigned int hsync_len;
    unsigned int vsync_len;
    uint32_t sync;
    uint32_t vmode;
    uint32_t flage;
};

struct video_modelist {
    struct list_head list;
    const struct video_mode *mode;
};

/**
 * video_device - represent videocard device
 *
 */
struct video_device {
    struct device *device;
    struct video_ops *ops;
    struct video_state state;
    struct video_screen screen;
    struct list_head modes;
    const struct video_mode *cur_mode;

    void *pseudo_palette;
    void *framebuffer;
    size_t frame_size;

#ifdef CONFIG_VIDEO_FOREIGN_ENDIAN
    uint32_t fb_bigendian:1;
#endif
};

/**
 * video_ops - videocard operations
 * @read:
 * @write:
 * @sync:
 * @mmap:
 * @blank: set blank mode
 * @panoff: set pan offset according to vdev->screen
 * @setmode: set all mode according to vdev->screen and update vdev->state
 * @imageblit:
 * @ioctl:
 */
struct video_ops {
    state (*open)(struct video_device *vdev);
    state (*release)(struct video_device *vdev);
    state (*read)(struct video_device *vdev);
    state (*write)(struct video_device *vdev);
    state (*sync)(struct video_device *vdev);
    state (*blank)(struct video_device *vdev, enum video_blank mode);
    state (*panoff)(struct video_device *vdev);
    state (*checkmode)(struct video_device *vdev, struct video_screen *info);
    state (*setmode)(struct video_device *vdev);
    state (*mmap)(struct video_device *vdev, struct vm_area *vm);
    state (*imageblit)(struct video_device *vdev, const struct video_image *image);
    state (*ioctl)(struct video_device *vdev, unsigned int cmd, unsigned long arg);
};

static inline bool video_need_convert(struct video_device *vdev)
{
#ifdef CONFIG_VIDEO_FOREIGN_ENDIAN
# ifdef CONFIG_ARCH_LITTLE_ENDIAN
    return vdev->fb_bigendian;
# else
    return !vdev->fb_bigendian;
# endif /* CONFIG_ARCH_LITTLE_ENDIAN */
#else
# ifdef CONFIG_ARCH_LITTLE_ENDIAN
    return false;
# else
    return true;
# endif /* CONFIG_ARCH_LITTLE_ENDIAN */
#endif /* CONFIG_VIDEO_FOREIGN_ENDIAN */
}

#define VIDEO_POS_LEFT(vdev, bpp) (             \
    video_need_convert(vdev) ?                  \
    (32 - (bpp)) << (shift) : 0                 \
)

#define VIDEO_SHIFT_LOW(vdev, shift, val) (     \
    video_need_convert(vdev) ?                  \
    (val) << (shift) : (val) >> (shift)         \
)

#define VIDEO_SHIFT_HIGH(vdev, shift, val) (    \
    video_need_convert(vdev) ?                  \
    (val) >> (shift) : (val) << (shift)         \
)

/* video base */
extern void video_imageblit(struct video_device *vdev, struct video_image *image);

/* video mode */
extern state video_modelist_create(struct list_head *head, const struct video_mode *mode, unsigned int num);
extern void video_modelist_destroy(struct list_head *head);
extern const struct video_mode *video_mode_best(struct video_device *vdev, struct video_screen *info);

/* video console */
extern state video_console_register(struct video_device *vdev);

/* video core */
extern state video_register(struct video_device *);
extern void video_unregister(struct video_device *);

/* convenience logging macros */
#define video_emerg(vdev, fmt, ...)  dev_emerg(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_alert(vdev, fmt, ...)  dev_alert(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_crit(vdev, fmt, ...)   dev_crit(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_err(vdev, fmt, ...)    dev_err(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_warn(vdev, fmt, ...)   dev_warn(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_notice(vdev, fmt, ...) dev_notice(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_info(vdev, fmt, ...)   dev_info(&(vdev)->device, fmt, ##__VA_ARGS__)
#define video_debug(vdev, fmt, ...)  dev_debug(&(vdev)->device, fmt, ##__VA_ARGS__)

#endif  /* _DRIVER_VIDEO_H_ */
