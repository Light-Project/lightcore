/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <ioops.h>
#include <driver/video/vga.h>

#define xres 80
#define yres 25

struct vga_text {
    uint8_t ch;
    uint8_t att;
} __packed;

static struct vga_text buff[yres][xres];
static unsigned char pos_x, pos_y;

#define vram_base ((void *)0xb8000)
#define roll_size (2 * xres * (yres - 1))

static inline void vga_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x + (pos_y * 80);
    outb(VGA_CRT_IC, VGA_CRTC_CURSOR_HI);
    outb(VGA_CRT_DC, cursor >> 8);
    outb(VGA_CRT_IC, VGA_CRTC_CURSOR_LO);
    outb(VGA_CRT_DC, cursor);
}

static inline void vga_flush(void)
{
    setup_bigreal();
    bigreal_memcpy(vram_base, buff, sizeof(buff));
}

static inline void vga_clear(int pos_y, int len)
{
    int pos_x;
    for (; len--; pos_y++)
    for (pos_x = 0; pos_x < xres; pos_x++) {
        buff[pos_y][pos_x].ch = '\0';
        buff[pos_y][pos_x].att = 0x07;
    }
}

static inline void vga_roll(void)
{
    memmove(&buff[0][0], &buff[1][0], roll_size);
    vga_clear(yres - 1, 1);
}

void video_print(const char *str)
{
    char ch;

    while ((ch = *str++) != '\0') {
        if (pos_y >= yres) {
            vga_roll();
            pos_y--;
        }

        if (ch == '\n') {
            pos_y++;
            pos_x = 0;
            continue;
        }

        buff[pos_y][pos_x++].ch = ch;
        if (pos_x >= xres) {
            pos_y++;
            pos_x = 0;
        }

        vga_cursor(pos_x, pos_y);
    }

    vga_flush();
}

void video_clear()
{
    vga_cursor(0, 0);
    vga_clear(0, yres);
    vga_flush();
}
