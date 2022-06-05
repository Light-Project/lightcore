/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <framedraw.h>
#include <kernel.h>
#include <export.h>

static const uint32_t imageblit_tab8[16] = {
    0x00000000, 0xff000000,
    0x00ff0000, 0xffff0000,
    0x0000ff00, 0xff00ff00,
    0x00ffff00, 0xffffff00,
    0x000000ff, 0xff0000ff,
    0x00ff00ff, 0xffff00ff,
    0x0000ffff, 0xff00ffff,
    0x00ffffff, 0xffffffff,
};

static const uint32_t imageblit_tab8_cvt[16] = {
    0x00000000, 0x000000ff,
    0x0000ff00, 0x0000ffff,
    0x00ff0000, 0x00ff00ff,
    0x00ffff00, 0x00ffffff,
    0xff000000, 0xff0000ff,
    0xff00ff00, 0xff00ffff,
    0xffff0000, 0xffff00ff,
    0xffffff00, 0xffffffff,
};

static const uint32_t imageblit_tab16[4] = {
    0x00000000, 0xffff0000,
    0x0000ffff, 0xffffffff,
};

static const uint32_t imageblit_tab16_cvt[4] = {
    0x00000000, 0x0000ffff,
    0xffff0000, 0xffffffff,
};

static const uint32_t imageblit_tab32[2] = {
    0x00000000, 0xffffffff,
};

static void imageblit_simple(const struct framedraw_info *info, const struct video_image *image,
                             void *base, uint32_t fgcolor, uint32_t bgcolor)
{
    unsigned int bpp = info->bpp, lsize = info->line_size;
    unsigned int spitch, ppw, bshift, wlimit, wcount, hcount, count, tlen;
    uint32_t eorx, fgx = fgcolor, bgx = bgcolor, bmask, colortab[16];
    const uint8_t *sline, *src = image->data;
    uint32_t *dline;
    const uint32_t *tab;

    spitch = BITS_TO_U8(image->width);
    ppw = BITS_PER_U32 / bpp;

    switch (bpp) {
        case 8:
            tab = info->frameswab ? imageblit_tab8_cvt : imageblit_tab8;
            tlen = ARRAY_SIZE(imageblit_tab8);
            break;

        case 16:
            tab = info->frameswab ? imageblit_tab16_cvt : imageblit_tab16;
            tlen = ARRAY_SIZE(imageblit_tab16);
            break;

        case 32:
            tab = imageblit_tab32;
            tlen = ARRAY_SIZE(imageblit_tab32);
            break;

        default:
            return;
    }

    for (count = ppw; count < ppw; ++count) {
        fgx = (fgx << bpp) | fgcolor;
        bgx = (bgx << bpp) | bgcolor;
    }

    bmask = BIT_LOW_MASK(ppw);
    eorx = fgx ^ bgx;
    wlimit = image->width / ppw;

    for (count = 0; count < tlen; ++count)
        colortab[count] = (tab[count] & eorx) ^ bgx;

    for (hcount = 0, bshift = BITS_PER_U8; hcount < image->height; ++hcount) {
        dline = base;
        sline = src;

        switch (ppw) {
            case 4:
                for (wcount = wlimit; wcount >= 2; wcount -= 2, ++sline) {
                    framedraw_writel(dline++, colortab[(*sline >> 4) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 0) & bmask]);
                }
                break;

            case 2:
                for (wcount = wlimit; wcount >= 4; wcount -= 4, ++sline) {
                    framedraw_writel(dline++, colortab[(*sline >> 6) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 4) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 2) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 0) & bmask]);
                }
                break;

            case 1:
                for (wcount = wlimit; wcount >= 8; wcount -= 8, ++sline) {
                    framedraw_writel(dline++, colortab[(*sline >> 7) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 6) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 5) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 4) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 3) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 2) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 1) & bmask]);
                    framedraw_writel(dline++, colortab[(*sline >> 0) & bmask]);
                }
                break;

            default:
                return;
        }

        while (wcount--) {
            bshift -= ppw;
            framedraw_writel(dline++, colortab[(*sline >> bshift) & bmask]);
            if (!bshift) {
                bshift = BITS_PER_U8;
                ++src;
            }
        }

        base += lsize;
        src += spitch;
    }
}

static void imageblit_complex(const struct framedraw_info *info, const struct video_image *image, void *base,
                              uint32_t fgcolor, uint32_t bgcolor, unsigned int start, unsigned int pitch)
{
    unsigned int bpp = info->bpp, lsize = info->line_size;
    unsigned int spitch, nbit, bshift, wcount, hcount;
    const uint8_t *sline, *src = image->data;
    uint32_t *dline, *pip = base;
    uint32_t shift, value;

    spitch = BITS_TO_U8(image->width);
    nbit = BITS_PER_U32 - bpp;

    for (hcount = 0; hcount < image->height; ++hcount) {
        shift = value = 0;
        dline = base;
        sline = src;

        if (start) {
            uint32_t smask = FRAMEDRAW_SHIFT_HIGH(info, shift, ~(uint32_t)0);
            value = framedraw_readl(dline) & smask;
            shift = start;
        }

        for (wcount = 0, bshift = BITS_PER_U8; wcount < image->width; ++wcount) {
            uint32_t color;

            color = (*sline & BIT(bshift--)) ? fgcolor : bgcolor;
            value |= FRAMEDRAW_SHIFT_HIGH(info, shift, color);

            if (shift >= nbit) {
                framedraw_writel(dline++, value);
                value = shift == nbit ? 0 :
                    FRAMEDRAW_SHIFT_LOW(info, color, BITS_PER_U32 - shift);
            }

            shift += bpp;
            shift &= BITS_PER_U32 - 1;

            if (!bshift) {
                bshift = BITS_PER_U8;
                sline++;
            }
        }

        if (shift) {
            uint32_t emask = FRAMEDRAW_SHIFT_HIGH(info, shift, ~(uint32_t)0);
            value = framedraw_readl(dline);
            framedraw_writel(dline, (value & emask) | value);
        }

        base += lsize;
        src += spitch;

        if (pitch) {
            pip += lsize;
            base = align_ptr_low(pip, sizeof(*pip));
            start += pitch;
            start &= BITS_PER_U32 - 1;
        }
    }
}

static void imageblit_color(const struct framedraw_info *info, const struct video_image *image,
                            void *base, unsigned int start, unsigned int pitch)
{
    unsigned int bpp = info->bpp, lsize = info->line_size;
    unsigned int spitch, nbit, hcount, wcount;
    const uint8_t *sline, *src = image->data;
    uint32_t *dline, *pip = base;
    uint32_t shift, value;

    spitch = BITS_TO_U8(image->width);
    nbit = BITS_PER_U32 - bpp;

    for (hcount = 0; hcount < image->height; ++hcount) {
        shift = value = 0;
        dline = base;
        sline = src;

        if (start) {
            uint32_t smask = FRAMEDRAW_SHIFT_HIGH(info, shift, ~(uint32_t)0);
            value = framedraw_readl(dline) & smask;
            shift = start;
        }

        for (wcount = 0; wcount < image->width; ++wcount) {
            uint32_t color;

            if (info->pseudo_color)
                color = info->pseudo_palette[*sline++];
            else
                color = *sline++;

            color <<= FRAMEDRAW_POS_LEFT(info, bpp);
            value |= FRAMEDRAW_SHIFT_HIGH(info, shift, color);

            if (shift >= nbit) {
                framedraw_writel(dline++, value);
                value = shift == nbit ? 0 :
                    FRAMEDRAW_SHIFT_LOW(info, color, BITS_PER_U32 - shift);
            }

            shift += bpp;
            shift &= BITS_PER_U32 - 1;
        }

        if (shift) {
            uint32_t emask = FRAMEDRAW_SHIFT_HIGH(info, shift, ~(uint32_t)0);
            value = framedraw_readl(dline);
            framedraw_writel(dline, (value & emask) | value);
        }

        base += lsize;
        src += spitch;

        if (pitch) {
            pip += lsize;
            base = align_ptr_low(pip, sizeof(*pip));
            start += pitch;
            start &= BITS_PER_U32 - 1;
        }
    }
}

void imageblit(const struct framedraw_info *info, const struct video_image *image)
{
    unsigned int bpp = info->bpp, lsize = info->line_size;
    unsigned int start, pitch, bitpos;
    void *base;

    bitpos = image->ypos * lsize * BITS_PER_U8 + image->xpos * bpp;
    start = bitpos & (BITS_PER_U32 - 1);
    pitch = (lsize & (BYTES_PER_U32 - 1)) * BITS_PER_U8;

    bitpos = (bitpos / BITS_PER_U8) & ~(BYTES_PER_U32 - 1);
    base = info->framebuffer + bitpos;

    if (image->depth > 1)
        imageblit_color(info, image, base, start, pitch);
    else {
        uint32_t fgcolor, bgcolor;

        if (info->pseudo_color) {
            fgcolor = info->pseudo_palette[image->fgcolor];
            bgcolor = info->pseudo_palette[image->bgcolor];
        } else {
            fgcolor = image->fgcolor;
            bgcolor = image->bgcolor;
        }

        if (!(bpp % BITS_PER_U32) && !start && !pitch &&
            !(image->width & (BITS_PER_U32 / bpp - 1) &&
             bpp >= 8 && bpp <= 32))
            imageblit_simple(info, image, base, fgcolor, bgcolor);
        else
            imageblit_complex(info, image, base, fgcolor, bgcolor, start, pitch);
    }
}
EXPORT_SYMBOL(imageblit);
