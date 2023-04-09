/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <framedraw.h>
#include <byteorder.h>
#include <export.h>

#if BITS_PER_LONG == 32
# define fillrect_read(addr) framedraw_readl(addr)
# define fillrect_write(addr, value) framedraw_writel(addr, value)
#else /* BITS_PER_LONG == 64 */
# define fillrect_read(addr) framedraw_readq(addr)
# define fillrect_write(addr, value) framedraw_writeq(addr, value)
#endif /* BITS_PER_LONG == 64 */

#define GENERIC_BITFILL_ALIGNED(name, single, operation)                                \
static void name(const struct framedraw_info *info, unsigned long *base,                \
                 unsigned int index, unsigned long color, unsigned int length)          \
{                                                                                       \
    unsigned long start, last;                                                          \
    unsigned long value;                                                                \
                                                                                        \
    if (!length)                                                                        \
        return;                                                                         \
                                                                                        \
    start = FRAMEDRAW_SHIFT_HIGH(info, index, ~0UL);                                    \
    last = ~FRAMEDRAW_SHIFT_HIGH(info, (index + length) % BITS_PER_LONG, ~0UL);         \
                                                                                        \
    if (index + length <= BITS_PER_LONG) {                                              \
        if (last)                                                                       \
            start &= last;                                                              \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, start));                     \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    if (start != ~0UL) {                                                                \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, start));                     \
        base++;                                                                         \
        length -= BITS_PER_LONG - index;                                                \
    }                                                                                   \
                                                                                        \
    for (length /= BITS_PER_LONG; length >= 8; length -= 8) {                           \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
        fillrect_write(base, operation); base++;                                        \
    }                                                                                   \
                                                                                        \
    while (length--) {                                                                  \
        fillrect_write(base, operation);                                                \
        base++;                                                                         \
    }                                                                                   \
                                                                                        \
    if (last) {                                                                         \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, last));                      \
    }                                                                                   \
}

#define GENERIC_BITFILL_UNALIGNED(name, single, operation)                              \
static void name(const struct framedraw_info *info, unsigned long *base,                \
                 unsigned int index, unsigned long color, unsigned int length,          \
                 unsigned int left, unsigned int right)                                 \
{                                                                                       \
    unsigned long start, last;                                                          \
    unsigned long value;                                                                \
                                                                                        \
    if (!length)                                                                        \
        return;                                                                         \
                                                                                        \
    start = FRAMEDRAW_SHIFT_HIGH(info, index, ~0UL);                                    \
    last = ~FRAMEDRAW_SHIFT_HIGH(info, (index + length) % BITS_PER_LONG, ~0UL);         \
                                                                                        \
    if (index + length <= BITS_PER_LONG) {                                              \
        if (last)                                                                       \
            start &= last;                                                              \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, start));                     \
        return;                                                                         \
    }                                                                                   \
                                                                                        \
    if (start != ~0UL) {                                                                \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, start));                     \
        base++;                                                                         \
        length -= BITS_PER_LONG - index;                                                \
        color = color << left | color >> right;                                         \
    }                                                                                   \
                                                                                        \
    for (length /= BITS_PER_LONG; length >= 4; length -= 4) {                           \
        fillrect_write(base, operation); base++;                                        \
        color = color << left | color >> right;                                         \
        fillrect_write(base, operation); base++;                                        \
        color = color << left | color >> right;                                         \
        fillrect_write(base, operation); base++;                                        \
        color = color << left | color >> right;                                         \
        fillrect_write(base, operation); base++;                                        \
        color = color << left | color >> right;                                         \
    }                                                                                   \
                                                                                        \
    while (length--) {                                                                  \
        fillrect_write(base, operation);                                                \
        base++;                                                                         \
        color = color << left | color >> right;                                         \
    }                                                                                   \
                                                                                        \
    if (last) {                                                                         \
        value = fillrect_read(base);                                                    \
        fillrect_write(base, framedraw_comb(single, value, last));                      \
    }                                                                                   \
}

GENERIC_BITFILL_ALIGNED(fillrect_copy_aligned, color, color)
GENERIC_BITFILL_UNALIGNED(fillrect_copy_unaligned, color, color)
GENERIC_BITFILL_ALIGNED(fillrect_xor_aligned, value ^ color, fillrect_read(base) ^ color)
GENERIC_BITFILL_UNALIGNED(fillrect_xor_unaligned, value ^ color, fillrect_read(base) ^ color)

void fillrect(const struct framedraw_info *info, const struct video_fillrect *fillrect)
{
    unsigned int bpp = info->bpp, lsize = info->line_size;
    unsigned int hcount, index, left, right, bits;
    unsigned long color, color2;
    unsigned long *base;

    if (info->pseudo_color)
        color = info->pseudo_palette[fillrect->color];
    else
        color = fillrect->color;

    color = framedraw_cpattern(bpp, color);
    base = align_ptr_low(info->framebuffer, BYTES_PER_LONG);
    index = ((uintptr_t)info->framebuffer & (BYTES_PER_LONG - 1)) * BITS_PER_U8;
    index += fillrect->ypos * lsize * BITS_PER_U8 + fillrect->xpos * bpp;
    left = BITS_PER_LONG % bpp;

    if (!left) {
        void (*func)(const struct framedraw_info *info, unsigned long *base, unsigned int index,
                     unsigned long color, unsigned int length);

        switch (fillrect->rop) {
            case VIDEO_ROP_XOR:
                func = fillrect_xor_aligned;
                break;

            case VIDEO_ROP_COPY: default:
                func = fillrect_copy_aligned;
                break;
        }

        for (hcount = 0; hcount < fillrect->height; ++hcount) {
            base += index >> ffsuf(BITS_PER_LONG);
            index &= BITS_PER_LONG - 1;
            func(info, base, index, color, fillrect->width * bpp);
            index += lsize * BITS_PER_U8;
        }
    } else {
        void (*func)(const struct framedraw_info *info, unsigned long *base, unsigned int index,
                     unsigned long color, unsigned int length, unsigned int left, unsigned int right);

        switch (fillrect->rop) {
            case VIDEO_ROP_XOR:
                func = fillrect_xor_unaligned;
                break;

            case VIDEO_ROP_COPY: default:
                func = fillrect_copy_unaligned;
                break;
        }

#ifdef CONFIG_ARCH_LITTLE_ENDIAN
        right = left;
        left = bpp - right;
#else /* CONFIG_ARCH_BIG_ENDIAN */
        right = bpp - left;
#endif /* CONFIG_ARCH_BIG_ENDIAN */

        for (hcount = 0; hcount < fillrect->height; ++hcount) {
            base += index / BITS_PER_LONG;
            index &= BITS_PER_LONG - 1;
            bits = index % bpp;
            color2 = le_long_to_cpu(framedraw_rol(cpu_to_le_long(color), bits, bpp));
            func(info, base, index, color2, fillrect->width * bpp, left, right);
            index += lsize * BITS_PER_U8;
        }
    }
}
EXPORT_SYMBOL(fillrect);
