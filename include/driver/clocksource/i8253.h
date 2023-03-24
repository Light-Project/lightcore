/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_CLOCKSOURCE_I8253_H_
#define _DRIVER_CLOCKSOURCE_I8253_H_

#include <bits.h>
#include <math.h>

/*
 * The Intel 8254 is a counter/timer device designed to solve the common timing
 * control problems in microcomputer system design. It provides three independent
 * 16-bit counters, each capable of handling clock inputs up to 10 MHz. All modes
 * are software programmable. The 8254 is a superset of the 8253.
 */

enum i8253_registers {
    I8253_COUNTER0      = 0x00, /* (RO) Channel 0 Data */
    I8253_LATCH0        = 0x00, /* (WO) Channel 0 Latch */
    I8253_STATUS0       = 0x00, /* (RO) @ Channel 0 Status */
    I8253_COUNTER1      = 0x01, /* (RO) Channel 1 Data */
    I8253_LATCH1        = 0x01, /* (WO) Channel 1 Latch */
    I8253_STATUS1       = 0x01, /* (RO) @ Channel 1 Status */
    I8253_COUNTER2      = 0x02, /* (RO) Channel 2 Data */
    I8253_LATCH2        = 0x02, /* (WO) Channel 2 Latch */
    I8253_STATUS2       = 0x02, /* (RO) @ Channel 2 Status */
    I8253_MODE          = 0x03, /* (WO) Timer Control Register */
    I8253_READBACK      = 0x03, /* (WO) @ Read Back Command */
};

/************************************************************************************/
/*      Mnemonic                    value              meaning/usage                */

/*
 * Each counter's status byte can be read following a Read Back Command. If latch status
 * is chosen (bit 4=0, Read Back Command) as a read back option for a given counter, the
 * next read from the counter's Counter Access Ports Register (40h for counter 0 and 42h
 * for counter 2) returns the status byte. The status byte returns the following:
 */

#define I8253_STATUS_PIN            BIT(7)          /* @ Output pin state */
#define I8253_STATUS_NULL           BIT(6)          /* @ Null count flags */
#define I8253_STATUS_ACCES          BIT_RANGE(5, 4) /* @ Read/Write Select Mask */
#define I8253_STATUS_MOD            BIT_RANGE(3, 1) /* @ Counter Mode Mask */
#define I8253_STATUS_CNT            BIT(0)          /* @ BCD/Binary Countdown Mask */

/*
 * This register is programmed prior to any counter being accessed to specify counter
 * modes. Following reset, the control words for each register are undefined and each
 * counter output is 0. Each timer must be programmed to bring it into a known state.
 * There are two special commands that can be issued to the counters through this
 * register, the Read Back Command and the Counter Latch Command. When these
 * commands are chosen, several bits within this register are redefined.
 */

/*
 * The Counter Latch Command latches the current count value. This command is used to
 * ensure that the count read from the counter is accurate. The count value is then read
 * from each counter's count register through the Counter Ports Access Ports Register
 * (40h for counter 0, 41h for counter 1, and 42h for counter 2). The count must be read
 * according to the programmed format, i.e., if the counter is programmed for two byte
 * counts, two bytes must be read. The two bytes do not have to be read one right after
 * the other (read, write, or programming operations for other counters may be inserted
 * between the reads). If a counter is latched once and then latched again before the
 * count is read, the second Counter Latch Command is ignored.
 */

#define I8253_MODE_SEL_READBACK     BIT_SHIFT(6, 3) /* @ Counter Select: Read Back Command */
#define I8253_MODE_SEL_TIMER2       BIT_SHIFT(6, 2) /* Counter Select: Counter 2 select */
#define I8253_MODE_SEL_TIMER1       BIT_SHIFT(6, 1) /* Counter Select: Counter 1 select */
#define I8253_MODE_SEL_TIMER0       BIT_SHIFT(6, 0) /* Counter Select: Counter 0 select */
#define I8253_MODE_ACCESS_WORD      BIT_SHIFT(4, 3) /* Read/Write Select: Read/Write LSB then MSB */
#define I8253_MODE_ACCESS_HIBYTE    BIT_SHIFT(4, 2) /* Read/Write Select: Read/Write Most Significant Byte */
#define I8253_MODE_ACCESS_LOBYTE    BIT_SHIFT(4, 1) /* Read/Write Select: Read/Write Least Significant Byte */
#define I8253_MODE_ACCESS_LATCH     BIT_SHIFT(4, 0) /* Read/Write Select: Counter Latch Command */
#define I8253_MODE_MOD_HTRIG        BIT_SHIFT(1, 5) /* Counter Mode: Hardware triggered strobe */
#define I8253_MODE_MOD_STRIG        BIT_SHIFT(1, 4) /* Counter Mode: Software triggered strobe */
#define I8253_MODE_MOD_WAVE         BIT_SHIFT(1, 3) /* Counter Mode: Square wave output */
#define I8253_MODE_MOD_RATE         BIT_SHIFT(1, 2) /* Counter Mode: Rate generator (divide by n counter) */
#define I8253_MODE_MOD_ONE          BIT_SHIFT(1, 1) /* Counter Mode: Hardware retriggerable one-shot */
#define I8253_MODE_MOD_CNT          BIT_SHIFT(1, 0) /* Counter Mode: Out signal on end of count (=0) */
#define I8253_MODE_CNT_BCD          BIT_SHIFT(0, 1) /* BCD/Binary Countdown: four-digit BCD */
#define I8253_MODE_CNT_BINARY       BIT_SHIFT(0, 0) /* BCD/Binary Countdown: 16-bit binary */

/*
 * The Read Back Command is used to determine the count value, programmed mode,
 * and current states of the OUT pin and Null count flag of the selected counter or
 * counters. Status and/or count may be latched in any or all of the counters by selecting
 * the counter during the register write. The count and status remain latched until read,
 * and further latch commands are ignored until the count is read.
 * Both count and status of the selected counters may be latched simultaneously by
 * setting both bit 5 and bit 4 to 0. If both are latched, the first read operation from that
 * counter returns the latched status. The next one or two reads, depending on whether
 * the counter is programmed for one or two byte counts, returns the latched count.
 * Subsequent reads return an unlatched count.
 */

#define I8253_READBACK_LCOUNT       BIT(5)          /* @ Latch Count of Selected Counters */
#define I8253_READBACK_LSTATUS      BIT(4)          /* @ Latch Status of Selected Counters */
#define I8253_READBACK_TIMER2       BIT(3)          /* @ Counter 2 count and/or status will be latched */
#define I8253_READBACK_TIMER1       BIT(2)          /* @ Counter 1 count and/or status will be latched */
#define I8253_READBACK_TIMER0       BIT(1)          /* @ Counter 0 count and/or status will be latched */

/* The clock frequency of the i8253/i8254 PIT */
#define I8253_CLKRATE               0x1234DE
#define I8253_LATCH(HZ)             DIV_ROUND_CLOSEST(I8253_CLKRATE, HZ)

#endif /* _DRIVER_CLOCKSOURCE_I8253_H_ */
