/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>
#include <asm/page.h>

#define IO_DELAY \
    outb %al, $0x80

#define printf(str) \
    movw $str, %si; \
    call Service_printf

    .code16
    .section    .head,"ax"
GLOBAL(startup16)
    cld
    cli

    /* Normalize the memory address */
    movw    %ds, %bx
    movw    %bx, %es
    movw    %bx, %ss
    movw    %bx, 1f
    .byte   0x66, 0xea  // ljmpl opcode
    .long   2f          // offset
1:  .word   0x00        // segment
2:

    /* Clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

    /* Disable NMI */
    movb    $0x80, %al
    outb    %al, $0x70
    IO_DELAY

    /* Test A20 first */
    call    a20test
    je      1f

    /* Bios way to enable A20 */
    printf(TXT_A20BIOS)
    movw    $0x2401, %ax
    int     $0x15

    call    a20test
    je      1f

    /* PCH way to enable A20 */
    printf(TXT_A20PCH)
    inb     $0x92, %al
    or      $(1<<1), %ax
    and     $(~(1<<0)), %ax
    outb    %al, $0x92

    call    a20test
    je      1f

    printf(TXT_A20ERROR)
    jmp     halt_16
1:

    /*
     * The startup header is meaningless
     * after boot, so we use it as a
     * temporary stack.
     */
    printf(TXT_MEMDETECT)
    movl    $_start, %esp
    calll   memdetect

    /* reset coprocessor */
    mov     $0xf0, %al
    outb    %al, $0xa1
    IO_DELAY
    mov     $0xf1, %al
    outb    %al, $0xa1
    IO_DELAY

    /* mask pic channel */
    movb    $0xff, %al
    outb    %al, $0xa1
    IO_DELAY
    movb    $0xfb, %al
    outb    %al, $0x21
    IO_DELAY

    /* set up the stack */
    movl    %ds, %ebx
    shll    $4, %ebx
    movl    $_ld_stack_end, %esp
    addl    %ebx, %esp

    /* load gdt table */
    addl    %ebx, gdt_fix
    lgdtl   gdt_struct

    /* load idt table */
    lidtl   idt_table

    /* enable pe mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    /* jmp to 32 */
    movl    %ds, %ebx
    shll    $4, %ebx
    addl    %ebx, 1f

    .byte   0x66, 0xea              // ljmpl opcode
1:  .long   startup32               // offset
    .word   GDT_ENTRY_BOOT_CS_BASE  // segment
END(startup16)

halt_16:
    cli
    rep;    hlt
    jmp     halt_16

a20test:
    ret

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
    pushw   %fs
    pushw   %gs
    pushw   %ds
    pushw   %es
    pushal

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

    .align  16
gdt_table:
    .long   0x00000000      // 0x00: NULL ENTRY
    .long   0x00000000      // 0x04: NULL
    .long   0x0000ffff      // 0x08: CS ENTRY
    .long   0x00cf9b00      // 0x0c: code, read/exe, 4GB, base=0
    .long   0x0000ffff      // 0x10: DS ENTRY
    .long   0x00cf9300      // 0x14: data, read/write, 4GB, base=0
gdt_struct:
    .word   0x0018          // GDT limit
gdt_fix: .long gdt_table    // GDT addr

    .align  16
idt_table:
    .word   0x0000          // IDT limit
    .long   0x00000000      // IDT addr

    .align  4
TXT_A20BIOS: .asciz "use bios to enable a20\n\r"
TXT_A20PCH: .asciz "use pch to enable a20\n\r"
TXT_A20ERROR: .asciz "fail to open a20\n\r"
TXT_MEMDETECT: .asciz "detect memory...\n\r"

/****************************/
/* The text section begins  */
/****************************/

    .code32
    .align  32
ENTRY(startup32)
    movl    $GDT_ENTRY_BOOT_DS_BASE, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    jmp     main
END(startup32)

GLOBAL(kernel_start)
    .byte   0xea                        // ljmpl opcode
1:  .long   pa_to_va(NORMAL_OFFSET)     // address
    .word   GDT_ENTRY_KERNEL_CS_BASE    // segment
END(kernel_start)

GLOBAL(halt)
    rep;    hlt
    jmp     halt
END(halt)
