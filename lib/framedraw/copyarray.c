/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <framedraw.h>
#include <export.h>

#if BITS_PER_LONG == 32
# define copyarray_read(addr) framedraw_readl(addr)
# define copyarray_write(addr, value) framedraw_writel(addr, value)
#else /* BITS_PER_LONG == 64 */
# define copyarray_read(addr) framedraw_readq(addr)
# define copyarray_write(addr, value) framedraw_writeq(addr, value)
#endif /* BITS_PER_LONG == 64 */

static void copyarray_forward(const struct framedraw_info *info, unsigned int length, unsigned long *dbase,
                              unsigned int dindex, const unsigned long *sbase, unsigned int sindex)
{
    unsigned long start, last;
    int shift;

    shift = dindex - sindex;
    start = FRAMEDRAW_SHIFT_HIGH(info, dindex, ~0UL);
    last = ~FRAMEDRAW_SHIFT_HIGH(info, (dindex + length) % BITS_PER_LONG, ~0UL);

    if (!shift) {
        if (dindex + length <= BITS_PER_LONG) {
            if (last)
                start &= last;
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), start));
            return;
        }

        if (start != ~0UL) {
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), start));
            dbase++;
            sbase++;
            length -= BITS_PER_LONG - dindex;
        }

        for (length /= BITS_PER_LONG; length >= 8; length -= 8) {
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
            copyarray_write(dbase++, copyarray_read(sbase++));
        }

        while (length--)
            copyarray_write(dbase++, copyarray_read(sbase++));

        if (last)
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), last));
    } else {
        unsigned long value, comb;
        int left, right, trailing;

        left = shift & (BITS_PER_LONG - 1);
        right = -shift & (BITS_PER_LONG - 1);

        if (dindex + length <= BITS_PER_LONG) {
            if (last)
                start &= last;
            value = copyarray_read(sbase);
            if (shift > 0)
                value <<= left;
            else if (sindex + length <= BITS_PER_LONG)
                value >>= right;
            else {
                comb = copyarray_read(sbase + 1);
                value = comb << left | right >> value;
            }
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));
            return;
        }

        value = copyarray_read(sbase++);
        if (shift > 0) {
            comb = value;
            value <<= left;
            length -= BITS_PER_LONG - dindex;
        } else {
            comb = copyarray_read(sbase++);
            value = comb << left | right >> value;
            length -= BITS_PER_LONG - dindex;
        }

        copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));
        dbase++;
        value = comb;
        trailing = length % BITS_PER_LONG;

        for (length /= BITS_PER_LONG; length >= 4; length -= 4) {
            copyarray_write(dbase++, (comb = copyarray_read(sbase++)) << left | right >> value); value = comb;
            copyarray_write(dbase++, (comb = copyarray_read(sbase++)) << left | right >> value); value = comb;
            copyarray_write(dbase++, (comb = copyarray_read(sbase++)) << left | right >> value); value = comb;
            copyarray_write(dbase++, (comb = copyarray_read(sbase++)) << left | right >> value); value = comb;
        }

        while (length--) {
            copyarray_write(dbase++, (comb = copyarray_read(sbase++)) << left | right >> value);
            value = comb;
        }

        if (trailing) {
            if (trailing <= BITS_PER_LONG - right)
                value >>= right;
            else {
                comb = copyarray_read(sbase);
                value = comb << left | right >> value;
            }
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));
        }
    }
}

static void copyarray_reverse(const struct framedraw_info *info, unsigned int length, unsigned long *dbase,
                              unsigned int dindex, const unsigned long *sbase, unsigned int sindex)
{
    unsigned long start, last;
    int shift;

    sbase += (sindex + length - 1) / BITS_PER_LONG;
    dbase += (dindex + length - 1) / BITS_PER_LONG;
    sindex = (sindex + length - 1) % BITS_PER_LONG;
    dindex = (dindex + length - 1) % BITS_PER_LONG;

    shift = dindex - sindex;
    start = ~FRAMEDRAW_SHIFT_HIGH(info, dindex, ~0UL);
    last = FRAMEDRAW_SHIFT_HIGH(info, (dindex + length) % BITS_PER_LONG, ~0UL);

    if (!shift) {
        if (dindex + 1 >= length) {
            if (start)
                last &= start;
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), last));
            return;
        }

        if (start) {
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), start));
            dbase--;
            sbase--;
            length -= dindex + 1;
        }

        for (length /= BITS_PER_LONG; length >= 8; length -= 8) {
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
            copyarray_write(dbase--, copyarray_read(sbase--));
        }

        while (length--)
            copyarray_write(dbase--, copyarray_read(sbase--));

        if (last != ~0UL)
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), copyarray_read(sbase), last));
    } else {
        unsigned long value, comb;
        int left, right, trailing;

        left = shift & (BITS_PER_LONG - 1);
        right = -shift & (BITS_PER_LONG - 1);

        if (dindex + length <= BITS_PER_LONG) {
            if (start)
                last &= start;
            value = copyarray_read(sbase);
            if (shift < 0)
                value >>= right;
            else if (sindex + 1 >= BITS_PER_LONG)
                value <<= left;
            else {
                comb = copyarray_read(sbase - 1);
                value = value << left | right >> comb;
            }
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));
            return;
        }

        value = copyarray_read(sbase--);
        if (shift < 0) {
            comb = value;
            value >>= left;
        } else {
            comb = copyarray_read(sbase++);
            value = value << left | right >> comb;
        }

        if (!start)
            copyarray_write(dbase, value);
        else
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));

        dbase--;
        value = comb;
        trailing = length % BITS_PER_LONG;

        for (length /= BITS_PER_LONG; length >= 4; length -= 4) {
            copyarray_write(dbase--, value << left | right >> (comb = copyarray_read(sbase--))); value = comb;
            copyarray_write(dbase--, value << left | right >> (comb = copyarray_read(sbase--))); value = comb;
            copyarray_write(dbase--, value << left | right >> (comb = copyarray_read(sbase--))); value = comb;
            copyarray_write(dbase--, value << left | right >> (comb = copyarray_read(sbase--))); value = comb;
        }

        while (length--) {
            copyarray_write(dbase--, value << left | right >> (comb = copyarray_read(sbase--)));
            value = comb;
        }

        if (trailing) {
            if (trailing <= BITS_PER_LONG - left)
                value <<= left;
            else {
                comb = copyarray_read(sbase);
                value = value << left | right >> comb;
            }
            copyarray_write(dbase, framedraw_comb(copyarray_read(dbase), value, start));
        }
    }
}

void copyarray(const struct framedraw_info *info, const struct video_copyarray *copyarray)
{
    unsigned int dx = copyarray->xdest, dy = copyarray->ydest;
    unsigned int sx = copyarray->xsrc, sy = copyarray->ysrc;
    unsigned int bpl = info->line_size * BITS_PER_U8;
    unsigned int sindex, dindex, hcount;
    unsigned long *base;
    bool reverse = false;

    if ((dy == sy && dx > sx) || dy > sy) {
        dy += copyarray->height;
        sy += copyarray->height;
        reverse = true;
    }

    base = align_ptr_low(info->framebuffer, BYTES_PER_LONG);
    dindex = sindex = ((uintptr_t)info->framebuffer & (BYTES_PER_LONG - 1)) * BITS_PER_U8;
    sindex += sy * bpl + sx * info->bpp;
    dindex += dy * bpl + dx * info->bpp;

    if (reverse) {
        for (hcount = 0; hcount < copyarray->height; ++hcount) {
            sindex -= bpl;
            dindex -= bpl;
            copyarray_reverse(
                info, copyarray->width * info->bpp,
                base + (dindex / BITS_PER_LONG), dindex % BITS_PER_LONG,
                base + (sindex / BITS_PER_LONG), sindex % BITS_PER_LONG
            );
        }
    } else {
        for (hcount = 0; hcount < copyarray->height; ++hcount) {
            copyarray_forward(
                info, copyarray->width * info->bpp,
                base + (dindex / BITS_PER_LONG), dindex % BITS_PER_LONG,
                base + (sindex / BITS_PER_LONG), sindex % BITS_PER_LONG
            );
            sindex += bpl;
            dindex += bpl;
        }
    }
}
EXPORT_SYMBOL(copyarray);
