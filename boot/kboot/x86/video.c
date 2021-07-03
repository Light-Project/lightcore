#include <kboot.h> 
#include <types.h>
#include <stddef.h>
#include <asm/io.h>

typedef struct{
    struct{
    uint8_t ch;
    uint8_t att;
    } block[26][80];
} __packed Vram_text_TypeDef;

#define Vram_text_Base  ((uint32_t)0xb8000)
#define vram_text       ((Vram_text_TypeDef *)Vram_text_Base)

#define CRTC_ADDR_REG   0x3D4
#define CRTC_DATA_REG   0x3D5
#define CURSOR_H        0xE
#define CURSOR_L        0xF

static void video_cursor(const char pos_x, const char pos_y)
{
    uint16_t cursor = pos_x+(pos_y*80);
    outb(CRTC_ADDR_REG, CURSOR_H);
    outb(CRTC_DATA_REG, (cursor >> 8) & 0xFF);
    outb(CRTC_ADDR_REG, CURSOR_L);
    outb(CRTC_DATA_REG, cursor & 0xFF);
}

static void video_roll()
{
    for(uint8_t pos_y = 1; pos_y <= 24; pos_y++)
    for(int8_t pos_x = 0; pos_x <= 79; pos_x++)
    {
        vram_text->block[pos_y-1][pos_x].ch = vram_text->block[pos_y][pos_x].ch;
        vram_text->block[pos_y-1][pos_x].att = vram_text->block[pos_y][pos_x].att;
    }   
}

void video_print(const char *str)
{
    static uint8_t pos_x = 0;
    static uint8_t pos_y = 0;
    
    if(str == NULL || *str == '\0')
        return;

    do{
        if(*str == '\n')
        {
            /* If the screen is full, scroll */
            if(*str == '\n' && pos_y == 23)
                video_roll();
            /* If the screen is not full, wrap */
            else
                pos_y++;
        }
        else if(*str == '\r')
            pos_x = 0;
        else{
            vram_text->block[pos_y][pos_x].ch = *str;
            vram_text->block[pos_y][pos_x].att = 0x07;
            pos_x++;
            video_cursor(pos_x, pos_y);
        }  
    }while(*(++str) != '\0');
}

void video_clear()
{
    for(uint8_t pos_y = 0; pos_y <= 24; pos_y++)
    for(int8_t pos_x = 0; pos_x <= 79; pos_x++)
    {
        vram_text->block[pos_y][pos_x].ch = '\0';
        vram_text->block[pos_y][pos_x].att = 0x07;
    }
    video_cursor(0, 0);
}
