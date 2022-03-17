#include <font.h>
#include <list.h>
#include <string.h>
#include <state.h>

SLIST_HEAD(font_head);

state font_register(struct font *font)
{
    slist_add(&font_head, &font->list);
    return -ENOERR;
}

state font_unregister(struct font *font)
{
    slist_del(&font_head, &font->list);
    return -ENOERR;
}

struct font *font_find(const char *name)
{
    struct font *font;

    if (!name)
        return NULL;

    slist_for_each_entry(font, &font_head, list) {
        if(!strcmp(font->name, name))
            return font;
    }

    return NULL;
}

struct font *font_suitable(unsigned int xres, unsigned int yres,
                           unsigned int wide, unsigned int height)
{
    struct font *font, *sui = NULL;
    unsigned int fontg, suig, val;

    suig = -10000;
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

    return sui;
}