/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "font"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <font.h>
#include <string.h>
#include <spinlock.h>
#include <printk.h>
#include <export.h>

static SLIST_HEAD(font_head);
static SPIN_LOCK(font_lock);

struct font *font_find_unlock(const char *name)
{
    struct font *font;

    slist_for_each_entry(font, &font_head, list) {
        if (!strcmp(font->name, name))
            return font;
    }

    return NULL;
}

struct font *font_find(const char *name)
{
    struct font *font;

    spin_lock(&font_lock);
    font = font_find_unlock(name);
    spin_unlock(&font_lock);

    return font;
}
EXPORT_SYMBOL(font_find);

state font_register(struct font *font)
{
    if (!font->name   || !font->data ||
        !font->height || !font->width)
        return -EINVAL;

    spin_lock(&font_lock);
    if (font_find_unlock(font->name)) {
        spin_unlock(&font_lock);
        pr_err("font '%s' already registered\n", font->name);
        return -EALREADY;
    }

    slist_add(&font_head, &font->list);
    spin_unlock(&font_lock);

    pr_debug("register font '%s'\n", font->name);
    return -ENOERR;
}
EXPORT_SYMBOL(font_register);

void font_unregister(struct font *font)
{
    pr_debug("unregister font '%s'\n", font->name);
    spin_lock(&font_lock);
    slist_del(&font_head, &font->list);
    spin_unlock(&font_lock);
}
EXPORT_SYMBOL(font_unregister);

struct font *font_suitable(unsigned int xres, unsigned int yres,
                           unsigned int wide, unsigned int height)
{
    struct font *font, *sui = NULL;
    unsigned int val, fontg, suig = -1000;

    spin_lock(&font_lock);
    slist_for_each_entry(font, &font_head, list) {
        fontg = font->pref;

        if ((yres < 400) == (font->height <= 8))
            fontg += 1000;

        val = (xres / font->width) * (yres / font->height) / 1000;
        if (val > 20)
            fontg += 20 - val;

        if ((wide & (1 << (font->width - 1))) &&
            (height & (1 << (font->height - 1))))
            fontg += 1000;

        if (fontg > suig) {
            suig = fontg;
            sui = font;
        }
    }
    spin_unlock(&font_lock);

    return sui;
}