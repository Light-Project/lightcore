/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCT_I8253_H_
#define _DRIVER_CLOCKSOURCT_I8253_H_

enum i8253_registers {
    I8253_COUNTER0    = 0x00, // Channel 0 data port (read/write)
    I8253_COUNTER1    = 0x01, // Channel 1 data port (read/write)
    I8253_COUNTER2    = 0x02, // Channel 2 data port (read/write)
    I8253_MODE        = 0x03, // Mode/Command register (write only, a read is ignored)
};

#define I8253_CLKRATE   0x1234DE
#define PIT_LATCH(HZ)   ((I8253_CLKRATE + (HZ) / 2) / (HZ))

/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */

/* Mode bitflag definitions */
#define I8253_MODE_SEL_READBACK   (3<<6)    // Select channel: Read-back command (8254 only)
#define I8253_MODE_SEL_TIMER2     (2<<6)    // Select channel: Channel 2
#define I8253_MODE_SEL_TIMER1     (1<<6)    // Select channel: Channel 1
#define I8253_MODE_SEL_TIMER0     (0<<6)    // Select channel: Channel 0

#define I8253_MODE_ACCESS_WORD    (3<<4)    // Access mode: lobyte/hibyte
#define I8253_MODE_ACCESS_HIBYTE  (2<<4)    // Access mode: hibyte only
#define I8253_MODE_ACCESS_LOBYTE  (1<<4)    // Access mode: lobyte only
#define I8253_MODE_ACCESS_LATCH   (0<<4)    // Latch count value command

#define I8253_MODE_MOD_HTRIG      (5<<1)    // Operating mode: hardware triggered strobe
#define I8253_MODE_MOD_STRIG      (4<<1)    // Operating mode: software triggered strobe
#define I8253_MODE_MOD_WAVE       (3<<1)    // Operating mode: square wave generator
#define I8253_MODE_MOD_RATE       (2<<1)    // Operating mode: rate generator
#define I8253_MODE_MOD_ONE        (1<<1)    // Operating mode: hardware re-triggerable one-shot
#define I8253_MODE_MOD_CNT        (0<<1)    // Operating mode: interrupt on terminal count

#define I8253_MODE_CNT_BCD        (1<<0)    // BCD/Binary mode: four-digit BCD
#define I8253_MODE_CNT_BINARY     (0<<0)    // BCD/Binary mode: 16-bit binary

/* Mode Read-back bitflag definitions */
#define I8253_READ_STATUS         (2<<4)    //
#define I8253_READ_VALUE          (1<<4)    //
#define I8253_READ_STATUSVALUE    (0<<4)    //
#define I8253_READ_COUNTER2       (1<<3)    //
#define I8253_READ_COUNTER1       (1<<2)    //
#define I8253_READ_COUNTER0       (1<<1)    //

/* Read-back flags (first) bitflag definitions */
#define I8253_STATUS_PIN          (1<<7)    // Output pin state
#define I8253_STATUS_NULL         (1<<6)    // Null count flags

#endif  /* _DRIVER_CLOCKSOURCT_I8253_H_ */
