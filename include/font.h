/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FONT_H_
#define _FONT_H_

#include <state.h>
#include <slist.h>
#include <math.h>

/**
 * struct font - describes a lattice font.
 * @width: width of a single word.
 * @height: height of a single word.
 * @limit: maximum representation range.
 * @pref: font suitable priority.
 */
struct font {
    struct slist_head list;
    const char *name;
    const void *data;
    unsigned int width;
    unsigned int height;
    unsigned int limit;
    int pref;
};

static inline const void *font_index(struct font *font, char ch)
{
    unsigned int bpc = DIV_ROUND_UP(font->width, 8) * font->height;
    return font->data + bpc * (ch & (font->limit - 1));
}

extern struct font *font_find(const char *name);
extern struct font *font_suitable(unsigned int xres, unsigned int yres, unsigned int wide, unsigned int height);
extern state font_register(struct font *font);
extern void font_unregister(struct font *font);

#endif  /* _FONT_H_ */
