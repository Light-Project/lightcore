/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FONT_H_
#define _FONT_H_

#include <state.h>
#include <slist.h>

/**
 * font - describes a font lib
 * @name: the name of font
 * @width: width of a single word
 * @height: height of a single word
 * @pref: font priority
 */
struct font {
    struct slist_head list;
    const void *name;
    unsigned int width;
    unsigned int height;
    unsigned int pref;
    const void *data;
};

extern struct font *font_find(const char *name);
extern struct font *font_suitable(unsigned int xres, unsigned int yres, unsigned int wide, unsigned int height);
extern state font_register(struct font *font);
extern state font_unregister(struct font *font);

#endif
