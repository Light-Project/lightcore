/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/smbus.h
 *  x86 smbus register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */

#ifndef _CHIP_X86_SMBUS_
#define _CHIP_X86_SMBUS_

#include <types.h>

typedef struct smbus{
    uint8_t hsts;       // 0x00: Host Status Register Address
    uint8_t Reserve0;   // 0x01: Reserve0
    uint8_t hctl;       // 0x02: Host Control Register
    uint8_t hcmd;       // 0x03: Host Command Register
    uint8_t tsa;        // 0x04: Transmit Slave Address Register
    uint8_t hd0;        // 0x05: Data 0 Register
    uint8_t hd1;        // 0x06: Data 1 Register
    uint8_t hbd;        // 0x07: Host Block Data
    uint8_t pec;        // 0x08: Packet Error Check Data Register
    uint8_t rsa;        // 0x09: Slave Address Register
    uint16_t sd;        // 0x0a: Slave Data Register
    uint8_t auxs;       // 0x0c: Auxiliary Status
    uint8_t auxc;       // 0x0d: Auxiliary Control
    uint8_t smlc;       // 0x0e: SMLINK_PIN_CTL Register
    uint8_t smbc;       // 0x0f: SMBUS_PIN_CTL Register
    uint8_t ssts;       // 0x10: Slave Status Register
    uint8_t scmd;       // 0x11: Slave Command Register
    uint8_t Reserve1[2];// 0x12: Reserve1
    uint8_t nda;        // 0x14: Notify Device Address Register
    uint8_t Reserve2;   // 0x15: Reserve2
    uint8_t ndlb;       // 0x16: Notify Data Low Byte Register
    uint8_t ndhb;       // 0x17: Notify Data High Byte Register
} smbus_t;


#endif /* _CHIP_X86_SMBUS_ */
