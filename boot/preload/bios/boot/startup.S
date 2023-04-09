/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

#define IO_DELAY \
    outb %al, $0x80

#define printf(str) \
    movw $str, %si; \
    call Service_printf

#define disk_check(dev) \
    movb dev, %dl;      \
    call Service_diskcheck

#define chs_read(dev, bufferoffset, buffer, count, sector, cylinder, head)  \
    movb dev, %dl; movw $bufferoffset, %ax; movw %ax, %es;                  \
    movw $buffer, %bx; movb $count, %al; movb $sector, %cl;                 \
    movb $cylinder, %ch; movb $head, %dh; call Service_chsdisk

/****************************/
/* The MBR section begins   */
/****************************/

    .code16
    .section .setup, "ax"

GLOBAL(_start)
    cld
    cli

    /* normalize memory address */
    ljmpl   $0x00, $1f
1:  xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss

    /* save device number */
    movw    %dx, boot_dev

    /* clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

    /* load full code */
    printf(TXT_MBR_BOOT)
    chs_read(boot_dev, 0, _ld_text_start, _ld_block_num, 2, 0, 0)
    jmp     init
END(_start)

/*
 * Screen Print Service:
 *  String Pointer: %SI
 */
Service_printf:
    movb    $0x0e, %ah
    movb    $0x03, %bl
    jmp     printf_loop
1:
    int     $0x10
printf_loop:
    lodsb   %es:(%si), %al
    cmpb    $0, %al
    jne     1b
    ret

/*
 * Disk Check Service:
 *  Disk Number:    %DL
 */
Service_diskcheck:
    movb    $0x01, %ah
    int     $0x13
    cmp     $0, %ah
    jne     INT13_1_ERR
    ret

/*
 * Disk CHS Reading Service:
 *  Service Number: %AH
 *  Sector Length:  %AL
 *  Buffer Addr:    %BX
 *  Cylinder:       %CH
 *  Sector:         %CL
 *  Head Number:    %DH
 *  Dev Number:     %DL
 *  Buffer Offset:  %ES
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

GLOBAL(boot_dev)
    .byte 0x00
END(boot_dev)

    .align  4
TXT_MBR_BOOT:       .asciz "[MBR] MBR booting\n\r"
TXT_INT13_1_ERR:    .asciz "[MBR] INT13/1 ERR!\n\r"
TXT_INT13_2_ERR:    .asciz "[MBR] INT13/2 ERR!\n\r"

/****************************/
/* The text section begins  */
/****************************/

    .code16
    .section .text, "ax"

ENTRY(init)
    /* clear the bss region */
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    xorl    %eax, %eax
    rep;    stosl %eax, %es:(%edi)

    /* setup stack */
    movl    $_ld_stack_end, %esp
    xorl    %ebp, %ebp

    jmp     main
END(init)

GLOBAL(setup_bigreal)
    /* load gdt and idt table */
    lgdtl   gdt_struct

    /* enable pe mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    /* tell 386/486 not crash */
    jmp     1f

1:
    /* select descriptor */
    movw    $0x08, %ax
    movw    %ax, %fs

    /* disable pe mode */
    movl    %cr0, %eax
    xorl    $(1<<0), %eax
    movl    %eax, %cr0

    retl
END(setup_bigreal)

GLOBAL(kboot_start)
    movl    4(%esp), %ecx
    movl    8(%esp), %edx
    pushl   %ecx
    pushl   %edx
    lretl
END(kboot_start)

GLOBAL(bios_int)
    pushl   %ebp
    movl    %esp, %ebp

    pushfl
    pushw   %ds
    pushw   %es
    pushal

    /* arg0: call number */
    movl    8(%ebp), %eax
    movb    %al, intnr

    /* arg2: save regs */
    movl    16(%ebp), %eax
    addl    $0x28, %eax
    pushl   %eax

    /* arg1: load regs */
    movl    %esp, %gs
    movl    12(%ebp), %esp

    /* pop register form ir */
    popal
    popw    %es
    popw    %ds
    popfl

    /* recovery sp */
    movl    %gs, %esp

    /* int imm8 */
    .byte   0xcd
intnr:  .byte  0

    /* pop save regs */
    popl    %esp

    /* push register to or */
    pushfl
    pushw   %ds
    pushw   %es
    pushal

    movl    %gs, %esp
    addl    $0x4, %esp

    popal
    popw    %es
    popw    %ds
    popfl

    leavel
    ret
END(bios_int)

    .align 16
    .section .rodata, "a"

SYM_DATA_START_LOCAL(gdt_table)
    .long   0x00000000  /* 0x00: NULL ENTRY                     */
    .long   0x00000000  /* 0x04: NULL                           */
    .long   0x0000ffff  /* 0x08: FS ENTRY                       */
    .long   0x00cf9300  /* 0x0c: data, read/write, 4GB, base=0  */
gdt_struct:
    .word   0x000f      /* GDT Limit    */
    .long   gdt_table   /* GDT Addr     */
SYM_DATA_END(gdt_table)
