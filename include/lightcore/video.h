/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_VIDEO_H_
#define _LIGHTCORE_VIDEO_H_

#include <lightcore/types.h>

#define VIDEO_SYNC_HOR_HIGH_ACT     0x01
#define VIDEO_SYNC_VERT_HIGH_ACT    0x02
#define VIDEO_SYNC_EXTERNAL         0x04
#define VIDEO_SYNC_COMP_HIGH_ACT    0x08
#define VIDEO_SYNC_BROADCAST        0x10
#define VIDEO_SYNC_ON_GREEN         0x20

#define VIDEO_VMODE_PROGRESSIVE     0x01
#define VIDEO_VMODE_INTERLACED      0x02
#define VIDEO_VMODE_DOUBLE          0x04
#define VIDEO_VMODE_ODD_FLD_FIRST   0x08

/**
 * video_bitfield -
 * @offset:
 * @length:
 * @msbr:
 */
struct video_bitfield {
    __u32 offset;
    __u32 length;
    unsigned char msbr;
};

/**
 * video_screen - video screen information
 * @xres: visible horizontal resolution
 * @yres: visible vertical resolution
 * @xres_virtual: virtual horizontal resolution
 * @yres_virtual: virtual vertical resolution
 * @xpos: horizontal pos from virtual to visible
 * @ypos: vertical pos from virtual to visible
 * @rotate: counter clockwise rotation angle
 * @bpp: bits per pixel
 * @red:
 * @green:
 * @blue:
 * @alpha:
 * @pixclk: pixel clock in pico seconds
 * @upper_margin: time from horizontal sync to picture
 * @lower_margin: time from picture to horizontal sync
 * @left_margin: time from vertical sync to picture
 * @right_margin: time from picture to vertical sync
 * @hsync_len: length of horizontal sync
 * @vsync_len: length of vertical sync
 * @vmode:
 */
struct video_screen {
    /* screen position */
    unsigned int xres;
    unsigned int yres;
    unsigned int xres_virtual;
    unsigned int yres_virtual;
    unsigned int xpos;
    unsigned int ypos;
    unsigned int rotate;
    unsigned int bpp;

    /* color field */
    struct video_bitfield red;
    struct video_bitfield green;
    struct video_bitfield blue;
    struct video_bitfield alpha;

    /* screen clock */
    unsigned int pixclk;
    unsigned int upper_margin;
    unsigned int lower_margin;
    unsigned int left_margin;
    unsigned int right_margin;
    unsigned int hsync_len;
    unsigned int vsync_len;
    __u32 sync;
    __u32 vmode;
};

/**
 * video_state - state provided by the video driver
 * @fbmem_start: framebuffer start physical address
 * @line_size: length of a line in bytes
 */
struct video_state {
	unsigned long fbmem_start;
	unsigned int xpanstep;
	unsigned int ypanstep;
    unsigned int line_size;
};

/**
 * video_blank - video blink mode
 * @disable: unblanked, hsync: on, vsync: on
 * @normal: blanked, hsync: on, vsync: on
 * @powerdown: blanked, hsync: off, vsync: off
 * @vsync: blanked, hsync: on, vsync: off
 * @hsync: blanked, hsync: off, vsync: on
 */
enum video_blank {
    VIDEO_BLANK_DISABLE,
    VIDEO_BLANK_NORMAL,
    VIDEO_BLANK_POWERDOWN,
    VIDEO_BLANK_VSYNC_SUSPEND,
    VIDEO_BLANK_HSYNC_SUSPEND,
};

/**
 * video_image -
 * @width: width of image
 * @height: height of image
 * @xpos: horizontal display position
 * @ypos: vertical display position
 * @depth: depth of the image
 * @data: image data
 */
struct video_image {
    unsigned int width;
    unsigned int height;
    unsigned int xpos;
    unsigned int ypos;
    unsigned char depth;
    __u32 front_color;
    __u32 back_color;
	const char *data;
};

/**
 * video_cursor - video hardware cursor
 *
 */
struct video_cursor {
    unsigned int enable;
    unsigned int xpos;
    unsigned int ypos;
    struct video_image image;
};

#endif /* _LIGHTCORE_VIDEO_H_ */
