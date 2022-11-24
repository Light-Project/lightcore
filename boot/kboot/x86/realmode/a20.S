/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <driver/platform/ibmpc.h>

#define I8042_DATA      0x60
#define I8042_BASE      0x64
#define I8042_OBF       0x01
#define I8042_IBF       0x02

#define I8042_DKDB      0xad
#define I8042_PREAD     0xd0
#define I8042_PWRITE    0xd1

#define A20_RETRY       500
#define A20_TEST_FAST   32
#define A20_TEST_SLOW   2097152
#define A20_TEST_ADDR   0x200

#define io_delay \
    outb    %al, $0x80

    .code16
    .section .text, "ax"

SYM_FUNC_START(a20_enable)
    movl    $A20_RETRY, %esi
L(a20e_retry):

    /* First, check to see if A20 is already enabled */
    movl    $A20_TEST_FAST, %ecx
    call    a20_test
    jne     L(a20e_finish)

/*
 * The best way to turn on the a20 is to use
 * the bios interrupt, so we try it first.
 */

    pushl   %esi

    /* A20-Gate Support */
    mov     $0x2403, %ax
    int     $0x15
    jb      bios_exit
    cmpb    $0, %ah
    jnz     bios_exit

    /* A20-Gate Status */
    mov     $0x2402, %ax
    int     $0x15
    jb      bios_exit
    cmpb    $0, %ah
    jnz     bios_exit

    /* A20-Gate check if activated */
    cmpb    $1, %al
    jz      bios_exit

    /* A20-Gate Activate */
    mov     $0x2401, %ax
    int     $0x15
    jb      bios_exit
    cmpb    $0, %ah
    jnz     bios_exit

bios_exit:
    popl    %esi

    movl    $A20_TEST_FAST, %ecx
    call    a20_test
    jne     L(a20e_finish)

/*
 * For the original method to enable the A20 line, some
 * hardware IO using the Keyboard Controller chip
 * (8042 chip) is necessary.
 */
    /* Disable keyboard */
    call    i8042_write_wait
    movb    $I8042_DKDB, %al
    outb    %al, $I8042_BASE

    /* Read from port */
    call    i8042_write_wait
    movb    $I8042_PREAD, %al
    outb    %al, $I8042_BASE

    call    i8042_read_wait
    inb     $I8042_DATA, %al
    orb     $0x02, %al
    pushw   %ax

    /* Write to port */
    call    i8042_write_wait
    movb    $I8042_PWRITE, %al
    outb    %al, $I8042_BASE

    call    i8042_write_wait
    popw    %ax
    outb    %al, $I8042_DATA

    /* Null command, but UHCI wants it */
    call    i8042_write_wait
    movb    $0xff, %al
    outb    %al, $I8042_BASE

    movl    $A20_TEST_SLOW, %ecx
    call    a20_test
    jne     L(a20e_finish)

/*
 * On most newer computers starting with the IBM PS/2,
 * the chipset has a FAST A20 option that can
 * quickly enable the A20 line.
 */
    inb     $IBMPC_FA20_BASE, %al
    orb     $IBMPC_FA20_GATE, %al
    andb    $IBMPC_FA20_INIT, %al
    outb    %al, $IBMPC_FA20_INIT

    movl    $A20_TEST_SLOW, %ecx
    call    a20_test
    jne     L(a20e_finish)

/*
 * On some systems reading ioport 0xee enables A20,
 * and writing it disables A20.
 */
    inb     $0xee, %al

    movl    $A20_TEST_SLOW, %ecx
    call    a20_test
    jne     L(a20e_finish)

    decl    %esi
    jnz     L(a20e_retry)
    movw    $0, %ax
    ret

L(a20e_finish):
    movw    $1, %ax
    ret
SYM_FUNC_END(a20_enable)

SYM_FUNC_START_LOCAL(a20_test)
    pushw   %es

    movl    $0xffff, %eax
    movl    %eax, %es

    movl    $A20_TEST_ADDR, %ebx
    movl    (%ebx), %edx

L(a20t_retry):
    incl    %edx
    movl    %edx, %ds:(%ebx)
    io_delay
    movl    %es:(%ebx), %eax

    cmpl    %eax, %edx
    jne     L(a20t_finish)
    loop    L(a20t_retry)

L(a20t_finish):
    popw    %es
    ret
SYM_FUNC_END(a20_test)

SYM_FUNC_START_LOCAL(i8042_read_wait)
    inb     $I8042_BASE, %al
    testb   $I8042_OBF, %al
    jz      i8042_read_wait
    ret
SYM_FUNC_END(i8042_read_wait)

SYM_FUNC_START_LOCAL(i8042_write_wait)
    inb     $I8042_BASE, %al
    testb   $I8042_IBF, %al
    jnz     i8042_write_wait
    ret
SYM_FUNC_END(i8042_write_wait)
