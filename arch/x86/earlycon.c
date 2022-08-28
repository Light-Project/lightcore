/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <console.h>
#include <driver/uart/8250.h>
#include <driver/video/vga.h>
#include <asm/proc.h>
#include <asm/page.h>
#include <asm/io.h>

#if defined(CONFIG_PRECON_VGA)

#define xres    80
#define yres    25

struct vga_text {
    uint8_t ch;
    uint8_t att;
} __packed;

static struct vga_text buff[yres][xres];
static unsigned char pos_x, pos_y;

#define vram_base (pa_to_va(0xb8000))
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
    memcpy(vram_base, buff, sizeof(buff));
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

        buff[pos_y][pos_x++].ch = ch;
        if (pos_x >= xres) {
            pos_y++;
            pos_x = 0;
        }

        vga_cursor(pos_x, pos_y);
    }

    vga_flush();
}

static struct console_ops vga_console_ops = {
    .write = vga_write,
};

static struct console vga_console = {
    .name = "early-vga",
    .ops = &vga_console_ops,
    .flags = CONSOLE_BOOT | CONSOLE_ENABLED,
};

void pre_console_init(void)
{
    vga_cursor(0, 0);
    vga_clear(0, yres);
    vga_flush();
    pre_console_register(&vga_console);
}

#elif defined(CONFIG_PRECON_SER)

#define I8250_BASE  0x3f8
#define I8250_FREQ  115200

static unsigned int serial_read(struct console *con, char *str, unsigned int len)
{
    unsigned int rlen;

    for (rlen = 0; len; --len, ++rlen) {
        if (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_DR))
            break;
        *str++ = inb(I8250_BASE + UART8250_THR);
    }

    return rlen;
}

static void serial_write(struct console *con, const char *str, unsigned int len)
{
    while (len--) {
        while (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_THRE))
            cpu_relax();
        outb(I8250_BASE + UART8250_THR, *str++);
    }
}

static void serial_sync(struct console *con)
{
    while (!(inb(I8250_BASE + UART8250_LSR) & UART8250_LSR_THRE))
        cpu_relax();
    outb(I8250_BASE + UART8250_FCR, UART8250_FCR_CLEAR_RCVR);
}

static struct console_ops ser_console_ops = {
    .read = serial_read,
    .write = serial_write,
    .sync = serial_sync,
};

static struct console ser_console = {
    .name = "early-serial",
    .ops = &ser_console_ops,
    .flags = CONSOLE_BOOT | CONSOLE_ENABLED,
};

void earlycon_init(void)
{
    uint16_t div = 115200 / I8250_FREQ;

    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_DLAB);
    outb(I8250_BASE + UART8250_DLL, div);
    outb(I8250_BASE + UART8250_DLH, div >> 8);

    outb(I8250_BASE + UART8250_IER, 0x00);
    outb(I8250_BASE + UART8250_MCR, UART8250_MCR_RTS | UART8250_MCR_DTR);
    outb(I8250_BASE + UART8250_FCR, UART8250_FCR_FIFO_EN);
    outb(I8250_BASE + UART8250_LCR, UART8250_LCR_WLS_8);

    earlycon_register(&ser_console);
}

#endif /* CONFIG_PRECON */
