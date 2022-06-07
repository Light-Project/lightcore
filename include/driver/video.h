/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_H_
#define _DRIVER_VIDEO_H_

#include <device.h>
#include <lightcore/video.h>

struct vmem_area;

/**
 * video_mode - video card supportive modes.
 * @name: video card mode name.
 * @refresh: screen refresh rate per second.
 * @xres: visible horizontal resolution.
 * @yres: visible vertical resolution.
 * @pixclk: pixel clock in pico seconds.
 * @upper_margin: time from horizontal sync to picture.
 * @lower_margin: time from picture to horizontal sync.
 * @left_margin: time from vertical sync to picture.
 * @right_margin: time from picture to vertical sync.
 * @hsync_len: length of horizontal sync.
 * @vsync_len: length of vertical sync.
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

/**
 * struct video_modelist - auxiliary struct for hanging mode on list.
 * @list: nodes attached to @modes list.
 * @mode: point to a video card mode.
 */
struct video_modelist {
    struct list_head list;
    const struct video_mode *mode;
};

/**
 * video_device - describe a video card device.
 * @dev: points to the parent device of the video card device.
 * @ops: operations method of video card device.
 * @list: list for centralized management of video card device.
 * @state: current video card infomations.
 * @screen: current video card parameters.
 * @modes: list of video card support modes.
 * @cur_mode: current video card mode.
 * @pseudo_palette: fake palette of 16 colors.
 * @framebuffer: address of frame buffer
 * @frame_size: size of frame buffer.
 */
struct video_device {
    struct device *dev;
    struct video_ops *ops;
    struct list_head list;

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
 * video_ops - describe the operations of a video card.
 * @enable: enable the video card.
 * @disable: disable the video card.
 * @sync: wait for video card blit idle.
 * @read: general video memory read operation.
 * @write: general video memory write operation.
 * @mmap: mapped video card memory.
 * @ioctl: perform video card specific ioctl.
 * @blank: blank display with video card hardware.
 * @panoff: pan display with video card hardware.
 * @setmode: set the video card parameters.
 * @checkmode: check parameter whether the video card supports.
 * @cursor: draws cursor with video card hardware.
 * @imageblit: draws image with video card hardware.
 * @fillrect: draws rectangle with video card hardware.
 * @copyarray: copy data in video card.
 */
struct video_ops {
    state (*enable)(struct video_device *vdev);
    state (*disable)(struct video_device *vdev);
    state (*sync)(struct video_device *vdev);
    state (*read)(struct video_device *vdev, void *buff, size_t count, loff_t *ppos);
    state (*write)(struct video_device *vdev, void *buff, size_t count, loff_t *ppos);
    state (*mmap)(struct video_device *vdev, struct vmem_area *vm);
    state (*ioctl)(struct video_device *vdev, unsigned int cmd, unsigned long arg);
    state (*blank)(struct video_device *vdev, enum video_blank mode);
    state (*panoff)(struct video_device *vdev);
    state (*setmode)(struct video_device *vdev);
    state (*checkmode)(struct video_device *vdev, struct video_screen *info);
    state (*cursor)(struct video_device *vdev, struct video_cursor *cursor);
    state (*imageblit)(struct video_device *vdev, const struct video_image *image);
    state (*fillrect)(struct video_device *vdev, const struct video_fillrect *fillrect);
    state (*copyarray)(struct video_device *vdev, const struct video_copyarray *copyarray);
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

extern spinlock_t video_lock;
extern struct list_head video_list;

/* video card operations */
extern state video_enable(struct video_device *vdev);
extern state video_disable(struct video_device *vdev);
extern state video_sync(struct video_device *vdev);
extern state video_read(struct video_device *vdev, void *buff, size_t count, loff_t *ppos);
extern state video_write(struct video_device *vdev, void *buff, size_t count, loff_t *ppos);
extern state video_ioctl(struct video_device *vdev, unsigned int cmd, unsigned long arg);
extern state video_mmap(struct video_device *vdev, struct vmem_area *vm);
extern state video_blank(struct video_device *vdev, enum video_blank mode);
extern state video_panoff(struct video_device *vdev);
extern state video_setmode(struct video_device *vdev);
extern state video_checkmode(struct video_device *vdev, struct video_screen *info);
extern state video_imageblit(struct video_device *vdev, const struct video_image *image);
extern state video_fillrect(struct video_device *vdev, const struct video_fillrect *fillrect);
extern state video_copyarray(struct video_device *vdev, const struct video_copyarray *copyarray);

/* video generic operations */
extern state video_cfbimageblit(struct video_device *vdev, const struct video_image *image);
extern state video_cfbfillrect(struct video_device *vdev, const struct video_fillrect *rect);
extern state video_cfbcopyarray(struct video_device *vdev, const struct video_copyarray *copyarray);

/* video mode */
extern state video_modelist_create(struct list_head *head, const struct video_mode *mode, unsigned int num);
extern void video_modelist_destroy(struct list_head *head);
extern const struct video_mode *video_mode_best(struct video_device *vdev, struct video_screen *info);

/* video console */
extern state video_console_register(struct video_device *vdev);
extern void video_console_unregister(struct video_device *vdev);

/* video core */
extern state video_register(struct video_device *);
extern void video_unregister(struct video_device *);

/* convenience logging macros */
#define video_emerg(vdev, fmt, ...)  dev_emerg(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_alert(vdev, fmt, ...)  dev_alert(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_crit(vdev, fmt, ...)   dev_crit(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_err(vdev, fmt, ...)    dev_err(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_warn(vdev, fmt, ...)   dev_warn(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_notice(vdev, fmt, ...) dev_notice(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_info(vdev, fmt, ...)   dev_info(&(vdev)->dev, fmt, ##__VA_ARGS__)
#define video_debug(vdev, fmt, ...)  dev_debug(&(vdev)->dev, fmt, ##__VA_ARGS__)

#endif  /* _DRIVER_VIDEO_H_ */
