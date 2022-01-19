/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/video.h>

// static const uint32_t cfb_tab8_le[] = {
//     0x00000000, 0xff000000,
//     0x00ff0000, 0xffff0000,
//     0x0000ff00, 0xff00ff00,
//     0x00ffff00, 0xffffff00,
//     0x000000ff, 0xff0000ff,
//     0x00ff00ff, 0xffff00ff,
//     0x0000ffff, 0xff00ffff,
//     0x00ffffff, 0xffffffff,
// };

// static const uint32_t cfb_tab8_be[] = {
//     0x00000000, 0x000000ff,
//     0x0000ff00, 0x0000ffff,
//     0x00ff0000, 0x00ff00ff,
//     0x00ffff00, 0x00ffffff,
//     0xff000000, 0xff0000ff,
//     0xff00ff00, 0xff00ffff,
//     0xffff0000, 0xffff00ff,
//     0xffffff00, 0xffffffff,
// };

// static const uint32_t cfb_tab16_le[] = {
//     0x00000000, 0xffff0000,
//     0x0000ffff, 0xffffffff,
// };

// static const uint32_t cfb_tab16_be[] = {
//     0x00000000, 0x0000ffff,
//     0xffff0000, 0xffffffff,
// };

// static const uint32_t cfb_tab32[] = {
//     0x00000000, 0xffffffff,
// };

// static void imageblit_simple(struct video_device *vdev, struct video_image *image)
// {
//     struct video_screen *screen = &vdev->screen;
//     unsigned int vcount, hcount;
//     const uint32_t *tab;
//     uint32_t *dest = vdev->framebuffer;

//     switch (screen->bpp) {
//         case 8:
//             tab = cfb_tab8_le;
//             break;
//         case 16:
//             tab = cfb_tab16_le;
//             break;
//         case 32: default:
//             tab = cfb_tab32;
//             break;
//     }

//     for (vcount = 0; vcount < image->height; ++vcount) {

//         for (hcount = 0; hcount; ++hcount) {


//         }
//     }
// }

// static void imageblit_complex(struct video_device *vdev, struct video_image *image)
// {

// }

static void imageblit_color(struct video_device *vdev, struct video_image *image,
                            void *buff, uint32_t start_index, uint32_t pitch_index)
{
    unsigned int vcount, hcount;
    unsigned int dpitch = vdev->state.line_size;
    unsigned int bpp = vdev->screen.bpp;
    unsigned int nbit = 32 - bpp;
    const char *src = image->data;
    uint32_t *dest, *pip = buff;
    uint32_t shift, color, val;

    for (vcount = 0; vcount < image->height; ++vcount) {
        shift = val = 0;
        dest = buff;

        if (start_index) {
            uint32_t smask = VIDEO_SHIFT_HIGH(vdev, shift, ~(uint32_t)0);
            val = *dest & smask;
            shift = start_index;
        }

        for (hcount = 0; hcount < image->width; ++hcount) {
            color = *src++;

            color <<= VIDEO_POS_LEFT(vdev, bpp);
            val |= VIDEO_SHIFT_HIGH(vdev, shift, color);
            if (shift >= nbit) {
                *dest++ = val;

            }

            shift += bpp;
            shift &= (32 - 1);
        }

        if (shift) {
            uint32_t emask = VIDEO_SHIFT_HIGH(vdev, shift, ~(uint32_t)0);
            *dest &= emask;
            *dest |= val;
        }

        buff += dpitch;

        if (pitch_index) {
            pip += dpitch;
            buff = align_ptr_low(pip, sizeof(*pip));
            start_index += pitch_index;
            start_index &= 32 - 1;
        }
    }
}

void video_imageblit(struct video_device *vdev, struct video_image *image)
{
    unsigned int line_size = vdev->state.line_size;
    unsigned int bpp = vdev->screen.bpp;
    uint32_t start_index, pitch_index, bitpos;
    void *framebuffer;

    bitpos = (image->ypos * line_size * 8) + (image->xpos * bpp);
    start_index = bitpos & (32 - 1);
    pitch_index = (line_size & (sizeof(uint32_t) - 1)) * 8;

	bitpos = (bitpos / 8) & ~(sizeof(uint32_t) - 1);
    framebuffer = vdev->framebuffer + bitpos;

    if (vdev->ops->sync)
        vdev->ops->sync(vdev);

    if (image->depth > 1)
        imageblit_color(vdev, image, framebuffer, start_index, pitch_index);
    // else {


    // }
}
