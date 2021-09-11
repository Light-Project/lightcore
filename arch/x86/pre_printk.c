/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <string.h>
#include <driver/video/vesa.h>
#include <console.h>

#include <asm/io.h>

#define xres    80
#define yres    25

struct _vram_text{
    struct{
    uint8_t ch;
    uint8_t att;
    } block[yres][xres];
} __packed;

#define vram_text_base  (pa_to_va(0xb8000))
#define vram_text       ((struct _vram_text *)vram_text_base)

static unsigned char pos_x, pos_y;

static inline void vga_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x + (pos_y * 80);
    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_HI);
    outb(VESA_CRT_DC, cursor >> 8);
    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_LO);
    outb(VESA_CRT_DC, cursor);
}

static inline void vga_clear(int pos_y, int len)
{
    int pos_x;
    for(; len--; pos_y++)
    for(pos_x = 0; pos_x < xres; pos_x++) {
        vram_text->block[pos_y][pos_x].ch = '\0';
        vram_text->block[pos_y][pos_x].att = 0x07;
    }
}

#define roll_size (2 * xres * (yres - 1))

static inline void vga_roll(void)
{
    memmove(&vram_text->block[0][0], &vram_text->block[1][0], roll_size);
    vga_clear(yres - 1, 1);
}

static void vga_write(struct console *con, const char *str, unsigned len)
{
    char ch;

    while ((ch = *str++) != '\0' && 0 < len--) {
        if (pos_y >= yres) {
            vga_roll();
            pos_y--;
        }

        if (ch == '\n') {
            pos_y++;
            pos_x = 0;
            continue;
        }

        vram_text->block[pos_y][pos_x++].ch = ch;
        if (pos_x >= xres) {
            pos_y++;
            pos_x = 0;
        }

        vga_cursor(pos_x, pos_y);
    }
}

static struct console_ops vga_console_ops = {
    .write = vga_write,
};

static struct console vga_console = {
    .name = "prevga",
    .ops = &vga_console_ops,
    .flags = CONSOLE_BOOT | CONSOLE_ENABLED,
};

void pre_printk_init(void)
{
    vga_clear(0, yres);
    vga_cursor(0, 0);
    pre_console_register(&vga_console);
}
