/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/pit.h
 *  x86 pit register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */

#ifndef _CHIP_X86_PIC_H_
#define _CHIP_X86_PIC_H_

#include <types.h>

enum Pit_Registers
{
    PIT_COUNTER0    = 0x40, // Channel 0 data port (read/write)
    PIT_COUNTER1    = 0x41, // Channel 1 data port (read/write)
    PIT_COUNTER2    = 0x42, // Channel 2 data port (read/write)
    PIT_MODE        = 0x43, // Mode/Command register (write only, a read is ignored)
};


/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */


/* Mode bitflag definitions */
#define PIT_MODE_SEL_READBACK   (3<<6)      // Select channel: Read-back command (8254 only)
#define PIT_MODE_SEL_TIMER2     (2<<6)      // Select channel: Channel 2
#define PIT_MODE_SEL_TIMER1     (1<<6)      // Select channel: Channel 1
#define PIT_MODE_SEL_TIMER0     (0<<6)      // Select channel: Channel 0

#define PIT_MODE_ACCESS_WORD    (3<<4)      // Access mode: lobyte/hibyte
#define PIT_MODE_ACCESS_HIBYTE  (2<<4)      // Access mode: hibyte only
#define PIT_MODE_ACCESS_LOBYTE  (1<<4)      // Access mode: lobyte only
#define PIT_MODE_ACCESS_LATCH   (0<<4)      // Latch count value command

#define PIT_MODE_MOD_HTRIG      (5<<1)      // Operating mode: hardware triggered strobe
#define PIT_MODE_MOD_STRIG      (4<<1)      // Operating mode: software triggered strobe
#define PIT_MODE_MOD_WAVE       (3<<1)      // Operating mode: square wave generator
#define PIT_MODE_MOD_RATE       (2<<1)      // Operating mode: rate generator
#define PIT_MODE_MOD_ONE        (1<<1)      // Operating mode: hardware re-triggerable one-shot
#define PIT_MODE_MOD_CNT        (0<<1)      // Operating mode: interrupt on terminal count

#define PIT_MODE_CNT_BCD        (1<<0)      // BCD/Binary mode: four-digit BCD
#define PIT_MODE_CNT_BINARY     (0<<0)      // BCD/Binary mode: 16-bit binary

/* Mode bitflag definitions */
#define PIT_READ_COUNTER0       (1<<1)
#define PIT_READ_COUNTER1       (1<<2)
#define PIT_READ_COUNTER2       (1<<3)
#define PIT_READ_STATUSVALUE    (0<<4)
#define PIT_READ_VALUE          (1<<4)
#define PIT_READ_STATUS         (2<<4)


#endif
