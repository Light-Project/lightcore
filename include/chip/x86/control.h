/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/control.h
 *  x86 control register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */
 
#ifndef _CHIP_X86_CONTROL_H_
#define _CHIP_X86_CONTROL_H_

enum Control_Registers
{
    CONTROL_XBUS                = 0x40, // Reset X-Bus IRQ12 And IRQ1 Register
    CONTROL_COPROCESSOR         = 0x41, // Coprocessor Error Register
    CONTROL_RESET               = 0xCF9,// Reset Control Register
    CONTROL_TIMER2              = 0x61, // Control Register
    CONTROL_NMI_STATE           = 0x61, // NMI Status
    CONTROL_NMI_CTL             = 0x70, // NMI Enable
    CONTROL_RTC                 = 0x70, // Real-Time Clock Address Register
}


/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */


#define CONTROL_RESET_RCPU      0x04        // Reset CPU
#define CONTROL_RESET_SRST      0x02        // System Reset


#define CONTROL_TIMER2_OUT      0x20        // Timer Counter 2 OUT Status

#define CONTROL_TIMER2_DAT_ON   0x02        // Timer Counter 2 Enable
#define CONTROL_TIMER2_DAT_Z    0x00        // Timer Counter 2 Enable

#define CONTROL_TIMER2_EN_DI       0x01        // Timer Counter 2 Enable
#define CONTROL_TIMER2_EN_EN       0x01        // Timer Counter 2 Enable

#endif
