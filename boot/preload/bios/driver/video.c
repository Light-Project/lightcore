/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <driver/video/vesa.h>

#include <asm/io.h>

#define xres    80
#define yres    25

struct _vram_text{
    struct{
    uint8_t ch;
    uint8_t att;
    } block[yres][xres];
} __packed;

#define VRAM_BASE   0xb8000
#define vram_text   ((struct _vram_text *)VRAM_BASE)

static unsigned char pos_x, pos_y;

static inline void video_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x + (pos_y * 80);

    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_HI);
    outb(VESA_CRT_DC, cursor >> 8);
    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_LO);
    outb(VESA_CRT_DC, cursor);
}

static void video_roll()
{
    for(uint8_t pos_y = 0; pos_y < 25; pos_y++)
    for(int8_t pos_x = 0; pos_x < 80; pos_x++) {
        ext_writeb(&vram_text->block[pos_y][pos_x].ch,
        ext_readb(&vram_text->block[pos_y + 1][pos_x].ch));
    }
}

void video_print(const char *str)
{
    char ch;

    while((ch = *str++) != '\0') {
        if (pos_y >= yres)
            video_roll(pos_y--);

        if (ch == '\n') {
            pos_y++;
            pos_x = 0;
            continue;
        }

        ext_writeb(&vram_text->block[pos_y][pos_x++].ch, ch);
        if (pos_x >= xres) {
            pos_y++;
            pos_x = 0;
        }

        video_cursor(pos_x, pos_y);
    }
}

void video_clear()
{
    for(pos_y = 0; pos_y < 25; pos_y++)
    for(pos_x = 0; pos_x < 80; pos_x++) {
        ext_writeb(&vram_text->block[pos_y][pos_x].ch, '\0');
        ext_writeb(&vram_text->block[pos_y][pos_x].att, 0x07);
    }

    video_cursor(0, 0);
    pos_y = pos_x = 0;
}
