/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/ps2.h
 *  x86 ps2 register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */

#ifndef _CHIP_X86_PS2_H_
#define _CHIP_X86_PS2_H_

#include <types.h>

enum Pit_Registers
{
    PS2_DATA        = 0x60, // Data Port (read/write)
    PS2_STATUS      = 0x64, // Status Register (read)
    PS2_CMD         = 0x42, // Command Register (write)
};

/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */

/* Status bitflag definitions */
#define PS_STATUS_PARITY_ERROR  0x80        // Parity error (0 = no error, 1 = parity error) 
#define PS_STATUS_TOUT          0x40        // Time-out error (0 = no error, 1 = time-out error) 
#define PS2_STATUS_MOD_CMD      0x08        // Command/data: 0 = data written to input buffer is data for PS/2 device
#define PS2_STATUS_MOD_DATA     0x00        // Command/data: 1 = data written to input buffer is data for PS/2 controller command
#define PS2_STATUS_SFLAG        0x04        // Meant to be cleared on reset and set by firmware
#define PS2_STATUS_IBUFF_FULL   0x02        // Input buffer status: full
#define PS2_STATUS_IBUFF_EMPTY  0x00        // Input buffer status: empty
#define PS2_STATUS_OBUFF_FULL   0x01        // Output buffer status: full
#define PS2_STATUS_OBUFF_EMPTY  0x00        // Output buffer status: empty


#endif
