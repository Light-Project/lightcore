/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot16.h>
#include <asm/io.h>
#include <driver/platform/ibmpc.h>
#include <driver/serio/i8042.h>

#define I8042_BASE      (0x60)
#define A20_RETRY       (500)
#define A20_TEST_FAST   (32)
#define A20_TEST_SLOW   (2097152)
#define A20_TEST_ADDR   (void *)(4 * 0x80)

static __always_inline uint8_t
i8042_in(unsigned int reg)
{
    return inb(I8042_BASE + reg);
}

static __always_inline void
i8042_out(unsigned int reg, uint8_t val)
{
    outb(I8042_BASE + reg, val);
}

static void i8042_wait(uint8_t mask, uint8_t pass)
{
    uint8_t val;

    for (;;) {
        val = i8042_in(I8042_STATUS);
        if ((val & mask) == pass)
            break;
    }
}

static inline void i8042_read_wait(void)
{
    return i8042_wait(I8042_STATUS_OBF, I8042_STATUS_OBF);
}

static inline void i8042_write_wait(void)
{
    return i8042_wait(I8042_STATUS_IBF, 0);
}

static void i8042_command(uint16_t cmd, uint8_t *para)
{
    int receive = (cmd >> 8) & 0x0f;
    int send = (cmd >> 12) & 0x0f;
    uint8_t *buff = para;

    i8042_write_wait();
    i8042_out(I8042_CMD, cmd);

    while (send--) {
        i8042_write_wait();
        i8042_out(I8042_DATA, *para++);
    }

    while (receive--) {
        i8042_read_wait();
        *buff++ = i8042_in(I8042_DATA);
    }
}

static void a20_kbd(void)
{
    uint8_t value;

    /*
     * For the original method to enable the A20 line,
     * some hardware IO using the Keyboard Controller
     * chip (8042 chip) is necessary.
     */
    i8042_command(I8042_CMD_KBD_DISABLE, NULL);
    i8042_command(I8042_CMD_PORT_INPUT, &value);
    i8042_command(I8042_CMD_PORT_OUTPUT, &(uint8_t){value | BIT(1)});
    i8042_command(I8042_CMD_KBD_ENABLE, NULL);

    /* Null command, but UHCI wants it */
    i8042_command(0xff, NULL);
}

static void a20_pch(void)
{
    uint8_t value;

    /*
     * On most newer computers starting with the IBM PS/2,
     * the chipset has a FAST A20 option that can
     * quickly enable the A20 line.
     */
    value = inb(IBMPC_FA20_BASE);
    value |= IBMPC_FA20_GATE;
    value &= IBMPC_FA20_INIT;
    outb(IBMPC_FA20_BASE, value);
}

static void a20_port(void)
{
    /*
     * On some systems reading ioport 0xee enables A20,
     * and writing it disables A20.
     */
    inb(IBMPC_QA20_BASE);
}

static void a20_bios(void)
{
    struct bios_reg or, ir = {};

    /* A20-Gate Support */
    ir.ax = 0x2403;
    bios_int(0x15, &ir, &or);
    if (or.ah) /* INT 15h is not supported */
        return;

    /* A20-Gate Status */
    ir.ax = 0x2402;
    bios_int(0x15, &ir, &or);
    if (or.ah) /* INT 15h couldn't get status */
        return;

    /* A20-Gate Activate */
    ir.ax = 0x2401;
    bios_int(0x15, &ir, &or);
    if (or.ah) /* INT 15h couldn't activate the gate */
        return;
}

static bool a20_test(unsigned int loop)
{
    uint32_t saved, val, result;

    saved = val = segment_readl(0x0000, A20_TEST_ADDR);

    while (loop--) {
        segment_writel(0x0000, A20_TEST_ADDR, ++val);
        io_delay();
        result = segment_readl(0xffff, A20_TEST_ADDR + 0x10);
        if (result != val)
            break;
    }

    segment_writel(0x0000, A20_TEST_ADDR, saved);
    return result != val;
}

int a20_enable(void)
{
    unsigned int count = A20_RETRY;

    while (count--) {
        if (a20_test(A20_TEST_FAST))
            return 0;

        a20_bios();
        if (a20_test(A20_TEST_FAST))
            return 0;

        a20_kbd();
        if (a20_test(A20_TEST_SLOW))
            return 0;

        a20_pch();
        if (a20_test(A20_TEST_SLOW))
            return 0;

        a20_port();
        if (a20_test(A20_TEST_SLOW))
            return 0;
    }

    return -1;
}
