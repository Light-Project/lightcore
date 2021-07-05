/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <mm.h>
#include <string.h>
#include <driver/video/vesa.h>
#include <printk.h>

#include <asm/io.h>

struct vram_text{
    struct{
    uint8_t ch;
    uint8_t att;
    } block[26][80];
} __packed;

#define vram_text_base  (pa_to_va(0xb8000))
#define vram_text       ((struct vram_text *)vram_text_base)

static unsigned char pos_x, pos_y;

static inline void video_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x + (pos_y * 80);

    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_HI);
    outb(VESA_CRT_DC, cursor >> 8);
    outb(VESA_CRT_IC, VESA_CRTC_CURSOR_LO);
    outb(VESA_CRT_DC, cursor);
}

static inline void video_roll()
{
    for(uint8_t pos_y = 1; pos_y <= 24; pos_y++)
    for(int8_t pos_x = 0; pos_x <= 79; pos_x++) {
        vram_text->block[pos_y-1][pos_x].ch = vram_text->block[pos_y][pos_x].ch;
        vram_text->block[pos_y-1][pos_x].att = vram_text->block[pos_y][pos_x].att;
    }   
}

static inline void video_print(char ch)
{
    if(ch == '\n') {
        /* If the screen is full, scroll */
        if(ch == '\n' && pos_y == 23)
            video_roll();
        /* If the screen is not full, wrap */
        else
            pos_y++;
    }
    
    else if(ch == '\r')
        pos_x = 0;
    
    else {
        vram_text->block[pos_y][pos_x].ch = ch;
        vram_text->block[pos_y][pos_x].att = 0x07;
        pos_x++;
        video_cursor(pos_x, pos_y);
    }
}

static inline void video_clear()
{
    memset(&vram_text->block, 0x00, sizeof(*vram_text));
    video_cursor(0, 0);
    pos_y = pos_x = 0;
}

static void pre_console_write(struct console *con, const char *str, unsigned len)
{
    while(len--)
        video_print(*str++);
}

static struct console pre_console_dev = {
    .name   = "precon",
    .write  = pre_console_write,
    .boot   = true,
    .index  = -1,
};

void pre_console_init(void)
{
    video_clear();
    pre_console = &pre_console_dev;
}
