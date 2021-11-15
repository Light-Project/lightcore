/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_XTENSA_ENTRY_H_
#define _ASM_XTENSA_ENTRY_H_

#ifdef __ASSEMBLY__

.macro SAVE_REGS
    wsr         a0, excsave2
    wsr         sp, excsave3

    addi        sp, sp, -0x50

    rsr         a0, epc1
    s32i        a0, sp, 0x00
    rsr         a0, ps
    s32i        a0, sp, 0x04
    rsr         a0, sar
    s32i        a0, sp, 0x08

    addi        sp, sp, 0x0c

    rsr         a0, excsave2
    s32i        a0, sp, 0x00
    rsr         a0, excsave3
    s32i        a0, sp, 0x04

    s32i        a2, sp, 0x08
    s32i        a3, sp, 0x0c
    s32i        a4, sp, 0x10
    s32i        a5, sp, 0x14
    s32i        a6, sp, 0x18
    s32i        a7, sp, 0x1c
    s32i        a8, sp, 0x20
    s32i        a9, sp, 0x24
    s32i        a10, sp, 0x28
    s32i        a11, sp, 0x2c
    s32i        a12, sp, 0x30
    s32i        a13, sp, 0x34
    s32i        a14, sp, 0x38
    s32i        a15, sp, 0x3c

    addi        sp, sp, -0x0c
.endm

.macro REC_REGS
    addi        sp, sp, 0x0c

    l32i        a0, sp, 0x00
    wsr         a0, excsave2
    l32i        a0, sp, 0x04
    wsr         a0, excsave3

    l32i        a2, sp, 0x08
    l32i        a3, sp, 0x0c
    l32i        a4, sp, 0x10
    l32i        a5, sp, 0x14
    l32i        a6, sp, 0x18
    l32i        a7, sp, 0x1c
    l32i        a8, sp, 0x20
    l32i        a9, sp, 0x24
    l32i        a10, sp, 0x28
    l32i        a11, sp, 0x2c
    l32i        a12, sp, 0x30
    l32i        a13, sp, 0x34
    l32i        a14, sp, 0x38
    l32i        a15, sp, 0x3c

    addi        sp, sp, -0x0c

    l32i        a0, sp, 0x00
    wsr         a0, epc1
    l32i        a0, sp, 0x04
    wsr         a0, ps
    l32i        a0, sp, 0x08
    wsr         a0, sar

    rsr         a0, excsave2
    rsr         sp, excsave3

    rsync
.endm

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_XTENSA_ENTRY_H_ */
