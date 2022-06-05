/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FRAMEDRAW_H_
#define _FRAMEDRAW_H_

#include <kernel.h>
#include <ioops.h>
#include <lightcore/video.h>

struct framedraw_info {
    unsigned int bpp;
    unsigned int line_size;
    uint32_t *pseudo_palette;
    void *framebuffer;
    bool pseudo_color;
    bool frameswab;
};

#define framedraw_readl(addr) readl(addr)
#define framedraw_readq(addr) readq(addr)
#define framedraw_writel(addr, value) writel(addr, value)
#define framedraw_writeq(addr, value) writeq(addr, value)

#define FRAMEDRAW_POS_LEFT(info, bpp) (             \
    (info)->frameswab ?                      \
    (32 - (bpp)) : 0                                \
)

#define FRAMEDRAW_SHIFT_LOW(info, shift, val) (     \
    (info)->frameswab ?                      \
    (val) << (shift) : (val) >> (shift)             \
)

#define FRAMEDRAW_SHIFT_HIGH(info, shift, val) (    \
    (info)->frameswab ?                      \
    (val) >> (shift) : (val) << (shift)             \
)

#if BITS_PER_LONG == 32
static inline unsigned long framedraw_cpattern(unsigned int bpp, uint32_t color)
{
    static const unsigned long pattern_table[] = {
        [1  / 2] = 0xffffffffUL,
        [2  / 2] = 0x55555555UL,
        [4  / 2] = 0x11111111UL,
        [8  / 2] = 0x01010101UL,
        [12 / 2] = 0x01001001UL,
        [14 / 2] = 0x00010001UL,
        [24 / 2] = 0x01000001UL,
        [32 / 2] = 0x00000001UL,
    };

    if (bpp / 2 > ARRAY_SIZE(pattern_table))
        return 0;

    return pattern_table[bpp / 2] * color;
}
#else /* BITS_PER_LONG == 64 */
static inline unsigned long framedraw_cpattern(unsigned int bpp, uint32_t color)
{
    static const unsigned long pattern_table[] = {
        [1  / 2] = 0xffffffffffffffffUL,
        [2  / 2] = 0x5555555555555555UL,
        [4  / 2] = 0x1111111111111111UL,
        [8  / 2] = 0x0101010101010101UL,
        [12 / 2] = 0x1001001001001001UL,
        [14 / 2] = 0x0001000100010001UL,
        [24 / 2] = 0x0001000001000001UL,
        [32 / 2] = 0x0000000100000001UL,
    };

    if (bpp / 2 > ARRAY_SIZE(pattern_table))
        return 0;

    return pattern_table[bpp / 2] * color;
}
#endif /* BITS_PER_LONG == 64 */

static inline unsigned long framedraw_comb(unsigned long a, unsigned long b, unsigned long mask)
{
    return ((a ^ b) & mask) ^ b;
}

static inline unsigned long framedraw_rol(unsigned long word, unsigned int shift, unsigned int limit)
{
    return (word << shift) | (word >> (limit - shift));
}

extern void imageblit(const struct framedraw_info *info, const struct video_image *image);
extern void fillrect(const struct framedraw_info *info, const struct video_fillrect *fillrect);

#endif  /* _FRAMEDRAW_H_ */
