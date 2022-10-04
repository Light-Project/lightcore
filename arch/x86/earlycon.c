/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <console.h>
#include <ioops.h>
#include <delay.h>

#if defined(CONFIG_PRECON_VGA)
# include <driver/serio/i8042.h>
# include <driver/input/atkbd.h>
# include <driver/video/vga.h>

#define xres    80
#define yres    25

struct vga_text {
    uint8_t ch;
    uint8_t att;
} __packed;

static struct vga_text buff[yres][xres];
static unsigned char pos_x, pos_y;
static uint8_t ps2ctl;
static bool atkbd_ready;

#define vram_base (pa_to_va(0xb8000))
#define roll_size (2 * xres * (yres - 1))

#define I8042_BASE 0x60
#define I8042_TIMEOUT 10000
#define I8042_BUFFER_SIZE 16

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

static bool i8042_wait(uint8_t mask, uint8_t pass)
{
    int timeout = I8042_TIMEOUT;
    uint8_t val;

    while (--timeout) {
        val = inb(I8042_BASE + I8042_STATUS);
        if ((val & mask) == pass)
            break;
        udelay(50);
    }

    return !!timeout;
}

static inline bool i8042_read_wait(void)
{
    return i8042_wait(I8042_STATUS_OBF, I8042_STATUS_OBF);
}

static inline bool i8042_write_wait(void)
{
    return i8042_wait(I8042_STATUS_IBF, 0);
}

static inline bool i8042_flush(void)
{
    unsigned int count;
    uint8_t value;

    for (count = 0; count < I8042_BUFFER_SIZE; ++count) {
        value = inb(I8042_BASE + I8042_STATUS);
        if (!(value & I8042_STATUS_OBF))
            return true;

        udelay(50);
        inb(I8042_BASE + I8042_DATA);
    }

    return false;
}

static state i8042_command(uint16_t cmd, uint8_t *param)
{
    unsigned int receive = PS2_GET_RECV(cmd);
    unsigned int send = PS2_GET_SEND(cmd);
    uint8_t *buff = param;

    if (!i8042_write_wait())
        return -EBUSY;
    outb(I8042_BASE + I8042_CMD, cmd);

    while (send--) {
        if (!i8042_write_wait())
            return -EBUSY;
        outb(I8042_BASE + I8042_DATA, *param++);
    }

    while (receive--) {
        if (!i8042_read_wait())
            return -EBUSY;
        *buff++ = inb(I8042_BASE + I8042_DATA);
    }

    return -ENOERR;
}

static int atkbd_recv(bool needack, unsigned int timeout)
{
    uint8_t value;

    while (--timeout) {
        value = inb(I8042_BASE + I8042_STATUS);
        if ((value & (I8042_STATUS_OBF | I8042_STATUS_AUXDATA))
            == I8042_STATUS_OBF) {
            value = inb(I8042_BASE + I8042_DATA);

            if (!needack)
                return value;
            if (value == ATKBD_RET_ACK)
                return value;
            if (value == ATKBD_RET_NAK)
                return value;
        }

        mdelay(1);
    }

    return -ETIMEDOUT;
}

static state atkbd_send(uint8_t cmd, unsigned int timeout)
{
    int retval;

    if (!i8042_write_wait())
        return -EBUSY;
    outb(I8042_BASE + I8042_DATA, cmd);

    retval = atkbd_recv(true, timeout);
    if (retval < 0)
        return retval;

    if (retval != ATKBD_RET_ACK)
        return -ENOTCONN;

    return -ENOERR;
}

static state atkbd_command(uint16_t cmd, uint8_t *param)
{
    unsigned int receive = PS2_GET_RECV(cmd);
    unsigned int send = PS2_GET_SEND(cmd);
    unsigned int count;
    state retval;
    uint8_t newctl;

    newctl = (ps2ctl | I8042_CTRL_AUXDIS | I8042_CTRL_KBDDIS)
             & ~(I8042_CTRL_KBDINT | I8042_CTRL_AUXINT);
    retval = i8042_command(I8042_CMD_CTL_WCTR, &newctl);
    if (retval)
        goto finish;

    newctl &= ~I8042_CTRL_KBDDIS;
    retval = i8042_command(I8042_CMD_CTL_WCTR, &newctl);
    if (retval)
        goto finish;

    switch (PS2_GET_CMD(cmd)) {
        case PS2_GET_CMD(ATKBD_CMD_RESET_BAT):
            /* Reset is special wrt timeouts */
            retval = atkbd_send(cmd, 1000);
            if (retval)
                goto finish;

            retval = atkbd_recv(false, 4000);
            if (retval < 0)
                goto finish;

            param[0] = retval;
            if (receive > 1) {
                retval = atkbd_recv(false, 500);
                if (retval < 0)
                    goto finish;
                param[1] = retval;
            }
            break;

        default:
            retval = atkbd_send(cmd, 200);
            if (retval)
                goto finish;

            for (count = 0; count < send; ++count) {
                retval = atkbd_send(param[count], 200);
                if (retval)
                    goto finish;
            }

            for (count = 0; count < receive; ++count) {
                retval = atkbd_recv(false, 500);
                if (retval < 0)
                    goto finish;
                param[count] = retval;
            }
            break;
    }

finish:
    return i8042_command(I8042_CMD_CTL_WCTR, &ps2ctl) ?: retval;
}

static unsigned int atkbd_read(struct console *con, char *str, unsigned int len)
{
    unsigned int rlen;

    for (rlen = 0; atkbd_ready && (rlen < len); ++rlen) {
        if (!(inb(I8042_BASE + I8042_STATUS) & I8042_STATUS_OBF))
            break;
        *str++ = inb(I8042_BASE + I8042_DATA);
    }

    return rlen;
}

static struct console_ops vga_console_ops = {
    .read = atkbd_read,
    .write = vga_write,
};

static struct console vga_console = {
    .name = "early-vga",
    .ops = &vga_console_ops,
    .flags = CONSOLE_BOOT | CONSOLE_ENABLED,
};

void earlycon_init(void)
{
    uint8_t result[2];
    unsigned int count;

    vga_cursor(0, 0);
    vga_clear(0, yres);
    vga_flush();

    /* Flush incoming keys (also verifies port is likely present) */
    if (!i8042_flush())
        goto finish;

    /* Disable keyboard / mouse and drain any input they may have sent */
    if (i8042_command(I8042_CMD_KBD_DISABLE, NULL))
        goto finish;
    if (i8042_command(I8042_CMD_AUX_DISABLE, NULL))
        goto finish;
    if (!i8042_flush())
        goto finish;

    /* Controller self-test */
    if (i8042_command(I8042_CMD_CTL_TEST, result))
        goto finish;
    if (result[0] != 0x55)
        goto finish;

    /* Controller keyboard test */
    if (i8042_command(I8042_CMD_KBD_TEST, result))
        goto finish;
    if (result[0] != 0x00)
        goto finish;

    /* Reset keyboard and self test */
    for (count = 0; count < 3; ++count) {
        if (!atkbd_command(ATKBD_CMD_RESET_BAT, result))
            break;
    }
    if (result[0] != 0xaa)
        goto finish;

    /* Disable keyboard */
    if (atkbd_command(ATKBD_CMD_RESET_DIS, result))
        goto finish;

    /* Set scancode command (mode 2) */
    result[0] = 0x02;
    if (atkbd_command(ATKBD_CMD_SSCANSET, result))
        goto finish;

    /* Enable keyboard */
    ps2ctl = I8042_CTRL_AUXDIS | I8042_CTRL_XLATE;
    if (atkbd_command(ATKBD_CMD_ENABLE, result))
        goto finish;

    atkbd_ready = true;

finish:
    earlycon_register(&vga_console);
}

#elif defined(CONFIG_PRECON_SER)
# include <driver/uart/8250.h>

#define I8250_BASE  0x3f8
#define I8250_FREQ  115200

static unsigned int serial_read(struct console *con, char *str, unsigned int len)
{
    unsigned int rlen;

    for (rlen = 0; rlen < len; ++rlen) {
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
