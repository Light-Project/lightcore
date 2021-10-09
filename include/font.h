/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FONT_H_
#define _FONT_H_

#include <slist.h>
#include <state.h>

struct font{
    const void *name;   /* Font name */
    int width, height;  /* Font size */
    const void *data;
    int pref;           /* Preference */
    slist_t list;
};

state font_register(struct font *font);
state font_unregister(struct font *font);

struct font *font_find(const char *name);
struct font *font_suitable(uint32_t xres, uint32_t yres, uint32_t font_w, uint32_t font_h);

#endif
