/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kboot.h>
#include <arch/x86/seg.h>
#include <arch/x86/regs.h>

#define io_delay \
    outb    %al, $0x80

#define printf(str) \
    movw    $str, %si; \
    call    Service_printf

    .code16
    .section .startup, "ax"

SYM_FUNC_START(startup16)
    cli
    cld

    /* Get the load address */
    calll   1f
1:  movl    (%esp), %ebx
    subl    $1b, %ebx

    movl    %cs, %eax
    shll    $4, %eax
    addl    %eax, %ebx

    /* Normalize the memory address */
    movl    %ebx, %eax
    shrl    $4, %eax

    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, 1f

    .byte   0x66, 0xea  /* ljmpl opcode */
    .long   2f          /* offset */
1:  .word   0x00        /* segment */
2:

    /* Save boot_offset first */
    movl    %ebx, boot_offset

    /* Clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

#ifdef CONFIG_ARCH_X86_64
/*
 * Tell the BIOS what CPU mode
 * we intend to run in.
 */
    movw    $0xec00, %ax
    movw    $0x0002, %bx
    int     $0x15
#endif

/*
 * Check whether there is enough
 * space in the bootparam.
 */
    movl    boot_offset, %ebx
    leal    _ld_bootparam_end(%ebx), %eax
    cmpl    $EBDA_START, %eax
    jle     1f

    printf(TXT_BOOTBIG)
    jmp     halt_16
1:

/*
 * We need to turn on A20 before we can load
 * the kernel into high-level memory
 */
    printf(TXT_A20ENABLE)
    call    a20_enable
    testw   %ax, %ax
    jnz     1f

    printf(TXT_A20ERROR)
    jmp     halt_16
1:

/*
 * We need to probe memory in real mode.
 */
    printf(TXT_MEMDETECT)
    call    memdetect

/*
 * We will use flat mode in later copies,
 * so we Enable the big real mode here.
 */

    /* Load GDT table */
    movl    boot_offset, %ebx
    addl    %ebx, gdt_struct + 2
    lgdtl   gdt_struct

    /* Enable PE mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    /* Tell 386/486 not crash */
    jmp     1f
1:

    /* Select descriptor */
    movw    $GDT_ENTRY_BOOT_DS_BASE, %ax
    movw    %ax, %es

    /* Disable PE mode */
    movl    %cr0, %eax
    xorl    $(1<<0), %eax
    movl    %eax, %cr0

/*
 * Check whether the kernel is loaded.
 */
    movl    code32_start, %edx
    movl    %es:(%edx), %eax
    cmpl    $PIGGY_MAGIC, %eax
    je      2f

/*
 * Running here shows that bootloader does not load
 * the kernel dispersedly. We manually split the kernel
 * to the specified address.
 */
    movl    boot_offset, %ebx
    leal    _ld_kernel_size(%ebx), %eax
    cmpl    $EBDA_START, %eax
    jle     1f

    movw    %ds, %ax
    movw    %ax, %es

    printf(TXT_PIGGYBIG)
    jmp     halt_16
1:

    leal    _ld_header_end(%ebx), %esi
    movl    $_ld_piggy_magic, %edi
    movl    $_ld_kernel_size, %ecx
    shrl    $2, %ecx
    rep;    movsl %es:(%esi), %es:(%edi)
2:

/*
 * Because of some problems with the compiler,
 * we can't use location independent, so we
 * copy the code to the absolute address.
 */
    movl    boot_offset, %esi
    movl    $_ld_header_start, %edi
    movl    $_ld_header_size, %ecx
    shrl    $2, %ecx
    rep;    movsl %es:(%esi), %es:(%edi)

    /* Disable NMI */
    movb    $0x80, %al
    outb    %al, $0x70
    io_delay

    /* Reset Coprocessor */
    mov     $0xf0, %al
    outb    %al, $0xa1
    io_delay
    mov     $0xf1, %al
    outb    %al, $0xa1
    io_delay

    /* Mask PIC Channel */
    movb    $0xff, %al
    outb    %al, $0xa1
    io_delay
    movb    $0xfb, %al
    outb    %al, $0x21
    io_delay

    /* Load GDT table */
    lgdtl   gdt_struct

    /* Load IDT table */
    lidtl   idt_table

    /* Enable PE mode */
    movl    %cr0, %eax
    orl     $CR0_PE, %eax
    movl    %eax, %cr0

    /* Jump to protected mode */
    ljmpl   $GDT_ENTRY_BOOT_CS_BASE, $startup32

SYM_FUNC_END(startup16)

SYM_FUNC_START(halt_16)
    cli
    rep;    hlt
    jmp     halt_16
SYM_FUNC_END(halt_16)

/*
 * Screen Print Service:
 *  String Pointer: %SI
 */
SYM_FUNC_START(Service_printf)
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
SYM_FUNC_END(Service_printf)

    .align  16
SYM_DATA_START_LOCAL(gdt_table)
    .long   0x00000000      /* 0x00: NULL ENTRY                     */
    .long   0x00000000      /* 0x04: NULL                           */
    .long   0x0000ffff      /* 0x08: CS ENTRY                       */
    .long   0x00cf9a00      /* 0x0c: code, read/exe, 4GB, base=0    */
    .long   0x0000ffff      /* 0x10: DS ENTRY                       */
    .long   0x00cf9200      /* 0x14: data, read/write, 4GB, base=0  */
gdt_struct:
    .word   0x0017          /* GDT Limit    */
    .long gdt_table         /* GDT Addr     */
SYM_DATA_END(gdt_table)

    .align  16
SYM_DATA_START_LOCAL(idt_table)
    .word   0x0000          /* IDT Limit    */
    .long   0x00000000      /* IDT Addr     */
SYM_DATA_END(idt_table)

    .align  4
SYM_DATA_START(boot_offset)
    .long   0x00000000
SYM_DATA_END(boot_offset)

    .align  4
TXT_BOOTBIG:    .asciz "[HEAD]: Boot head too big...\n\r"
TXT_PIGGYBIG:   .asciz "[HEAD]: Piggy data too big...\n\r"
TXT_MEMDETECT:  .asciz "[HEAD]: Detect memory...\n\r"
TXT_A20ENABLE:  .asciz "[HEAD]: Try to open a20\n\r"
TXT_A20ERROR:   .asciz "[HEAD]: Fail to open a20\n\r"
