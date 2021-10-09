/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  boot/preload/bios/boot/startup.S
 *  x86 preload entry
 *  (C) 2020 wzl
 *
 *  Change Logs:
 *  Date            Notes
 *  2021-01-18      first version
 *
 */

#include <linkage.h>
#include <arch/x86/seg.h>

#define printf(str)             \
        movw $str, %si;         \
        call Service_printf

#define disk_check(dev)         \
        movb dev, %dl;          \
        call Service_diskcheck

#define chs_read(dev, bufferoffset, buffer, count, sector, cylinder, head)  \
        movb dev, %dl;                                                      \
        movw $bufferoffset, %ax; movw %ax, %es;                             \
        movw $buffer, %bx;                                                  \
        movb $count, %al;                                                   \
        movb $sector, %cl; movb $cylinder, %ch; movb $head, %dh;            \
        call Service_chsdisk

/****************************/
/* The MBR section begins   */
/****************************/

    .code16
    .section .setup,"ax"
GLOBAL(_start)

    /* Disable interrupt */
    cli

    /* Normalize the memory address */
    ljmpl   $0x00, $1f
1:  xorl    %eax, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    /* setup stack */
    movl    $_ld_setup_start, %esp

    /* Save device number */
    movw    %dx, BOOT_DEV

    /* clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

    printf(TXT_MBR_BOOT)

    /* Load full code */
    chs_read(BOOT_DEV, 0, _ld_text_start, _ld_block_num, 2, 0, 0)
    jmp     init

END(_start)

/*
 * Print string to screen
 *
 * String pointer:  %si
 */
Service_printf:
    movb    $0x0e, %ah      // Service number
    movb    $0x03, %bl
    jmp     printf_loop
1:
    int     $0x10
printf_loop:
    lodsb
    cmpb    $0, %al
    jne     1b
    ret

/*
 * Hard disk check service:
 *
 * Disk number:     %dl
 */
Service_diskcheck:
    movb    $0x01, %ah
    int     $0x13
    cmp     $0, %ah
    jne     INT13_1_ERR
    ret

/*
 * disk chs reading service:
 *
 * Service number:  %AH
 * Sector length:   %AL
 * Buffer addr:     %BX
 * Cylinder:        %CH
 * Sector:          %CL
 * Head number:     %DH
 * Dev number:      %DL
 * Buffer offset:   %ES
 */
Service_chsdisk:
    mov     $0x02, %ah
    int     $0x13
    cmp     $0, %ah
    jne     INT13_1_ERR
    ret

INT13_1_ERR:
    printf(TXT_INT13_1_ERR)
    jmp     halt
INT13_2_ERR:
    printf(TXT_INT13_2_ERR)
    jmp     halt

GLOBAL(halt)
    cli
    rep;    hlt
    jmp     halt
END(halt)

    .globl BOOT_DEV
BOOT_DEV:
    .byte 0x00  // Startup device number

    .align 4
TXT_MBR_BOOT:       .asciz "\n\rMBR booting\n\r"
TXT_INT13_1_ERR:    .asciz "INT13/1 ERR!\n\r"
TXT_INT13_2_ERR:    .asciz "INT13/2 ERR!\n\r"

/****************************/
/* The text section begins  */
/****************************/

    .section .text,"ax"
init:
    /* TEST A20 first */
    call    Service_A20TEST
    je      1f

    /* Bios way to enable A20 */
    printf(TXT_A20BIOS)
    movw    $0x2401, %ax
    int     $0x15

    /* TEST A20 */
    call    Service_A20TEST
    je      1f

    /* PCH way to enable A20 */
    printf(TXT_A20PCH)
    inb     $0x92, %al
    or      $(1<<1), %ax    /* Enable A20 */
    and     $(~(1<<0)), %ax /* Diable reset */
    outb    %al, $0x92

    /* TEST A20 */
    call    Service_A20TEST
    je      1f
    printf(TXT_A20ERROR)
    jmp     halt
1:
    /**
     * TODO: setfs doesn't work
     */
    jmp     Bss_segment_Init

/*
 * We need access 4GiB in real mode
 */
setfs:
    /* Load gdt table */
    lgdtl   gdt_struct

    /* Enable PE mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    .byte   0x66, 0xea  # ljmpl opcodeq
    .long   1f          # offset
    .word   0x08        # segment

    .code32
1:
    movw    $0x10, %ax
    movw    %ax, %fs

    /* Disable PE mode */
    movl    %cr0, %eax
    xorl    $(1<<0), %eax
    movl    %eax, %cr0

1:
    .byte   0xea
    .word   1f - 1b
    .word   0x00

    .code16
1:

Bss_segment_Init:
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    xorl    %eax, %eax
    rep;    stosl %eax, %es:(%edi)

    xorl    %ebp, %ebp
    jmp     main

GLOBAL(kboot_start)
    movl    4(%esp), %eax
    movw    %ax, 1f
    movw    %ax, %ds
    movw    %ax, %es

    .byte   0x66, 0xea  # ljmpl opcode
    .long   0x200       # address
1:  .word   0x00        # offset
END(kboot_start)

GLOBAL(bios_int)

    /* Save env */
    pushl   %ebp
    movl    %esp, %ebp
    pushfl
    pushw   %fs
    pushw   %gs
    pushw   %ds
    pushw   %es
    pushal

    movl    $1, %eax
    movl    $2, %ebx
    addl    %eax, %ebx

    /* arg0: call number */
    movl    8(%ebp), %eax
    movb    %al, intnr

    /* arg2: Save regs */
    movl    16(%ebp), %eax
    addl    $0x2c, %eax
    pushl   %eax

    /* arg1: load regs */
    movl    %esp, save_sp
    movl    12(%ebp), %esp  /* arg1: input register */

    /* Pop register form ir */
    popal
    popw    %gs
    popw    %fs
    popw    %es
    popw    %ds
    popfl

    /* Recovery sp */
    movl    %cs:save_sp, %esp

    /* int imm8.  */
    .byte   0xcd
intnr:  .byte   0

    popl    %esp

    /* Push register to or */
    pushfl
    pushw   %ds
    pushw   %es
    pushw   %fs
    pushw   %gs
    pushal

    movl    save_sp, %esp
    addl    $0x4, %esp

    /* Recovery env */
    popal
    popw    %es
    popw    %ds
    popw    %gs
    popw    %fs
    popfl
    leavel
    ret

save_sp: .long  0x00
END(bios_int)

GLOBAL(ext_rw)
    pushl   %ebp
    movl    %esp, %ebp
    pushl   %ebx
    pushl   %ecx
    pushl   %edx

    /* arg0 flags - %esi */
    movl    8(%ebp), %esi

    /* arg1 addr - %bx */
    movl    12(%ebp), %ecx
    movw    %cx, %bx
    shrl    $4, %ecx
    andw    $0xf000, %cx
    movw    %cx, %es

    /* arg2 data - %edx */
    movl    16(%ebp), %edx

    xorl    %eax, %eax

    cmpl    $1, %esi
    jne     1f
    movb    %es:(%bx), %al
    jmp     2f
1:
    cmpl    $2, %esi
    jne     1f
    movw    %es:(%bx), %ax
    jmp     2f
1:
    cmpl    $3, %esi
    jne     1f
    movl    %es:(%bx), %eax
    jmp     2f
1:
    cmpl    $4, %esi
    jne     1f
    movb    %dl, %es:(%bx)
    jmp     2f
1:
    cmpl    $5, %esi
    jne     1f
    movw    %dx, %es:(%bx)
    jmp     2f
1:
    cmpl    $6, %esi
    jne     2f
    movl    %edx, %es:(%bx)

2:
    popl    %edx
    popl    %ecx
    popl    %ebx
    leavel
    ret
END(ext_rw)

ENTRY(delayus)
    pushl   %ebp
    movl    %esp, %ebp
    pushal

    movl    8(%ebp), %eax

    movw    %ax, %dx
    shrl    $16, %eax
    movw    %ax, %cx

    movb    $0x86, %ah
    int     $0x15

    popal
    leavel
    ret
END(delayus)

ENTRY(Service_A20TEST)
    pusha
    pushl   0x80 * 4

    call    io_delay

    popl    0x80 * 4
    popa
    ret
END(Service_A20TEST)

ENTRY(io_delay)
    outw    %ax, $0x80
    ret
END(io_delay)

TXT_A20BIOS:    .asciz "Open A20 through BIOS\n\r"
TXT_A20PCH:     .asciz "Open A20 through PCH\n\r"
TXT_A20ERROR:   .asciz "A20 can't open\n\r"

    .align 16
gdt_table:              // GDT Table
    .long   0x00000000  // 0x00: NULL ENTR
    .long   0x00000000  // 0x04: NULL
    .long   0x0000ffff  // 0x08: CS ENTRY
    .long   0x00cf9b00  // 0x0c: code, read/exe, 4GB, base=0
    .long   0x0000ffff  // 0x10: FS ENTRY
    .long   0x00cf9300  // 0x14: data, read/write, 4GB, base=0
gdt_struct:
    .word   0x0018      // GDT limit
    .long   gdt_table   // GDT addr
