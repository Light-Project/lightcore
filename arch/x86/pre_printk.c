/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <mm.h>
#include <string.h>
#include <driver/video/vesa.h>
#include <printk.h>

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

static inline void video_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x + (pos_y * 80);

    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_HI);
    outb(VESA_CRT_DC, cursor >> 8);
    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_LO);
    outb(VESA_CRT_DC, cursor);
}

static inline void video_clear(int pos_y, int len)
{
    int pos_x;
    for(; len--; pos_y++)
    for(pos_x = 0; pos_x < 80; pos_x++) {
        vram_text->block[pos_y][pos_x].ch = '\0';
        vram_text->block[pos_y][pos_x].att = 0x07;
    }
}

static inline void video_roll()
{
    memmove(&vram_text->block[0][0], &vram_text->block[1][0], 2 * xres * (yres - 1));
    video_clear(yres - 1, 1);
}

static void console_write(struct console *con, const char *str, unsigned len)
{
    char ch;
    
    while(len--, ch = *str++) {
        if(ch == '\n') {
            if(pos_y < (yres - 1))
                pos_y++;
            else 
                video_roll();
        } else if(ch == '\r') {
            pos_x = 0;
        } else if(pos_x < (xres - 1)) {
            vram_text->block[pos_y][pos_x++].ch = ch;
        } else
            continue;
        video_cursor(pos_x, pos_y);
    }
}

static inline void console_clear()
{
    video_clear(0, yres);
    video_cursor(0, 0);
}

static struct console pre_console_dev = {
    .name   = "precon",
    .write  = console_write,
    .boot   = true,
    .index  = -1,
};

void pre_console_init(void)
{
    console_clear();
    pre_console = &pre_console_dev;
}
