/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IMAGEBLIT_H_
#define _IMAGEBLIT_H_

#include <types.h>
#include <ioops.h>
#include <lightcore/video.h>

struct imageblit_info {
    unsigned int bpp;
    unsigned int line_size;
    uint32_t *pseudo_palette;
    void *framebuffer;
    bool pseudo_color;
    bool framebuffer_swab;
};

#define imageblit_readl(addr) readl(addr)
#define imageblit_writel(addr, value) writel(addr, value)
extern void imageblit(struct imageblit_info *info, struct video_image *image);

#endif  /* _IMAGEBLIT_H_ */
