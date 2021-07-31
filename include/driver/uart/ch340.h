/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _DRIVER_UART_CH340_H_
#define _DRIVER_UART_CH340_H_

#include <bits.h>

enum ch340_req {
    CH340_REQ_READ_VERSION  = 0x5f, //
    CH340_REQ_WRITE_REG     = 0x9a, //
    CH340_REQ_READ_REG      = 0x95, //
    CH340_REQ_SERIAL_INIT   = 0xa1, //
    CH340_REQ_MODEM_CTRL    = 0xa4, //
};

enum ch340_register {
    CH340_REG_BREAK         = 0x05,     //
    CH340_REG_PRESCALER     = 0x12,     //
    CH340_REG_DIVISOR       = 0x13,     // Divisor Latch Register
    CH340_REG_LCR           = 0x18,     //
    CH340_REG_LCR2          = 0x25,     //
    CH340_REG_STATUS        = 0x706,    // 
};

/*
 * The device line speed is given by the following equation:
 *
 *	baudrate = 48000000 / (2^(12 - 3 * ps - fact) * div), where
 *
 *		0 <= ps <= 3,
 *		0 <= fact <= 1,
 *		2 <= div <= 256 if fact = 0, or
 *		9 <= div <= 256 if fact = 1
 */
#define CH340_CLKRATE   48000000
#define CH340_CLK_DIV(ps, fact) (1 << (12 - 3 * (ps) - (fact)))

/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */

/* BREAK bitflag definitions */
#define CH340_BREAK_BITS            0x01

/* BREAK bitflag definitions */
#define CH340_LCR_ENABLE_RX         0x80        // Enable receive
#define CH340_LCR_ENABLE_TX         0x40        // Enable transport
#define CH340_LCR_MARK_SPACE        0x20        // Mark or space parity
#define CH340_LCR_PAR_EVEN          0x10        // Even Parity Select
#define CH340_LCR_ENABLE_PAR        0x08        // Parity enable
#define CH340_LCR_STOP_BITS_2       0x04        // Number of stop Bits, off = 1, on =  2)
#define CH340_LCR_CS8               0x03        // 8 bit character length
#define CH340_LCR_CS7               0x02        // 7 bit character length
#define CH340_LCR_CS6               0x01        // 6 bit character length
#define CH340_LCR_CS5               0x00        // 5 bit character length

#endif  /* _DRIVER_UART_CH340_H_ */
