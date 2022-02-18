/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

#define IO_DELAY \
    outb %al, $0x80

#define printf(str) \
    movw $str, %si; \
    call Service_printf

    .code16
    .section    .startup, "ax"
GLOBAL(startup16)
    cli
    cld

    /* Get the load address */
    calll   1f
1:  movl    (%esp), %ebx
    subl    $1b, %ebx

    movl    %cs, %eax
    shll    $4, %eax
    addl    %eax, %ebx
    movl    %ebx, boot_offset

    /* Normalize the memory address */
    movl    %ebx, %eax
    shrl    $4, %eax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    movw    %ax, 1f

    .byte   0x66, 0xea  /* ljmpl opcode */
    .long   2f          /* offset */
1:  .word   0x00        /* segment */
2:

    /* Clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

    /* Disable NMI */
    movb    $0x80, %al
    outb    %al, $0x70
    IO_DELAY

/*
 * The startup header is meaningless
 * after boot, so we use it as a
 * temporary stack.
 */
    printf(TXT_MEMDETECT)
    movl    $sentinel, %esp
    calll   memdetect

    printf(TXT_A20ENABLE)
    calll   a20_enable
    cmpl    $0, %eax
    je      1f
    printf(TXT_A20ERROR)
    jmp     halt_16
1:

/*
 * Reset Coprocessor
 */
    mov     $0xf0, %al
    outb    %al, $0xa1
    IO_DELAY
    mov     $0xf1, %al
    outb    %al, $0xa1
    IO_DELAY

/*
 * Mask PIC Channel
 */
    movb    $0xff, %al
    outb    %al, $0xa1
    IO_DELAY
    movb    $0xfb, %al
    outb    %al, $0x21
    IO_DELAY

    /* load gdt table */
    movl    boot_offset, %ebx
    addl    %ebx, gdt_fix
    lgdtl   gdt_struct

    /* load idt table */
    lidtl   idt_table

    /* enable pe mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    /* jmp to 32 */
    movl    %ds, %eax
    shll    $4, %eax
    addl    %eax, 1f

    .byte   0x66, 0xea              // ljmpl opcode
1:  .long   startup32               // offset
    .word   GDT_ENTRY_BOOT_CS_BASE  // segment
END(startup16)

halt_16:
    cli
    rep;    hlt
    jmp     halt_16

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

    .align  16
gdt_table:
    .long   0x00000000      /* 0x00: NULL ENTRY                     */
    .long   0x00000000      /* 0x04: NULL                           */
    .long   0x0000ffff      /* 0x08: CS ENTRY                       */
    .long   0x00cf9b00      /* 0x0c: code, read/exe, 4GB, base=0    */
    .long   0x0000ffff      /* 0x10: DS ENTRY                       */
    .long   0x00cf9300      /* 0x14: data, read/write, 4GB, base=0  */
gdt_struct:
    .word   0x0018          /* GDT Limit    */
gdt_fix: .long gdt_table    /* GDT Addr     */

    .align  16
idt_table:
    .word   0x0000          /* IDT Limit    */
    .long   0x00000000      /* IDT Addr     */

    .align  4
boot_offset:
    .long   0x00000000

    .align  4
TXT_MEMDETECT:  .asciz "[HEAD]: Detect memory...\n\r"
TXT_A20ENABLE:  .asciz "[HEAD]: Try to open a20\n\r"
TXT_A20ERROR:   .asciz "[HEAD]: Fail to open a20\n\r"
