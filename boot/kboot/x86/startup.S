/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <arch/x86/seg.h>
#include <linkage.h>

#define A20_ADDR    (4*0x80)
#define A20_FAST    (0x000100)
#define A20_FULL    (0x200000)

#define printf(str)             \
        movw $str, %si;         \
        call Service_printf

    .code16
    .section .startup,"ax"
GLOBAL(startup16)
    
    /* Disable interrupt at first */
    cli
    
    /* Normalize the memory address */
    movw    %ds, %bx
    movw    %bx, 1f
    .byte   0x66, 0xea  # ljmpl opcode
    .long   2f          # offset
1:  .word   0x00        # segment
2:  xorw    %ax, %ax
    movw    %ax, %ss
    movl    $_ld_stack_end, %esp
    shll    $4, %ebx
    addl    %ebx, %esp
    
    /* clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10
    
    /* Disable NMI */
    movb    $0x80, %al
    outb    %al, $0x70  
    call    IO_DELAY  

    /* TEST A20 first */
    call    a20test
    je      1f

    /* Bios way to enable A20 */
    printf(TXT_A20BIOS)
    movw    $0x2401, %ax
    int     $0x15

    /* TEST A20 */
    call    a20test
    je      1f

    /* PCH way to enable A20 */
    printf(TXT_A20PCH)
    inb     $0x92, %al
    or      $(1<<1), %ax    /* Enable A20 */
    and     $(~(1<<0)), %ax /* Diable reset */
    outb    %al, $0x92

    /* TEST A20 */
    call    a20test
    je      1f
    printf(TXT_A20ERROR)
    jmp     halt_16
1:

    /*
     * Before entering the protected 
     * mode, we can detect the memory 
     * through the BIOS
     */
    #call    mem_detect

    /* reset coprocessor */
    mov     $0xf0, %al
    outb    %al, $0xa1
    call    IO_DELAY
    mov     $0xf1, %al
    outb    %al, $0xa1
    call    IO_DELAY

    /* mask pic */
    movb    $0xff, %al
    outb    %al, $0xa1
    call    IO_DELAY
    movb    $0xfb, %al
    outb    %al, $0x21
    call    IO_DELAY
    
    /* Load gdt table */
    movl    %ds, %ebx
    shll    $4, %ebx
    addl    %ebx, gdt_fix
    lgdtl   gdt_struct
    
    /* Load idt table */
    lidtl   idt_table

    /* Enable PE mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0
    
    /* Jmp to 32 */
    movl    %ds, %ebx
    shll    $4, %ebx
    addl    %ebx, 1f

    .byte   0x66, 0xea              # ljmpl opcode
1:  .long   startup32               # offset
    .word   GDT_ENTRY_BOOT_CS_BASE  # segment

IO_DELAY:
    outb    %al, $0x80
    ret
    
a20test:

    ret

mem_detect:
    movw    $0xe820, %ax
    movl    $0x534d4150,%edx    /* ASCII "SMAP" */
    movw    (e820_pt), %di      /* e820 after int vector */
    movw    $0x14, %cx

    int     $0x15

    cmpl    $0x534d4150, %eax   /* BIOS messed up map */
    jne     1f

    /* e820 over */
    cmpl    $0, %eax
    jne     1f
    addw    $0x18, e820_pt
    
    jmp     mem_detect
1:
    ret
e820_pt: .word e820_table
    
Service_printf:
    movb    $0x0e, %ah
    movb    $0x03, %bl
    jmp     printf_loop
1:
    int     $0x10
printf_loop:
    lodsb
    cmpb    $0, %al
    jne     1b
    ret    
    
halt_16:
    hlt
    jmp     halt_16

e820_table:
.rept 64
    .long   0   // 0x00: addr
    .long   0
    .long   0   // 0x08: size
    .long   0
    .long   0   // 0x14: type
.endr

    .align 16
gdt_table:              // GDT Table
    .long   0x00000000  // 0x00: NULL ENTR
    .long   0x00000000  // 0x04: NULL
    .long   0x0000ffff  // 0x08: CS ENTRY
    .long   0x00cf9b00  // 0x0c: code, read/exe, 4GB, base=0
    .long   0x0000ffff  // 0x10: DS ENTRY
    .long   0x00cf9300  // 0x14: data, read/write, 4GB, base=0
gdt_struct:
            .word   0x0018      // GDT limit
gdt_fix:    .long   gdt_table   // GDT addr

    .align 16
idt_table:
    .word   0x0000      // IDT limit
    .long   0x00000000  // IDT addr

TXT_A20BIOS:    .asciz "Open A20 through BIOS\n"
TXT_A20PCH:     .asciz "Open A20 through PCH\n"
TXT_A20ERROR:   .asciz "A20 can't open\n"

END(startup16)

/****************************/
/* The text section begins  */
/****************************/

    .code32
    .align 32
ENTRY(startup32)

    /* Set data selector */
    movl    $GDT_ENTRY_BOOT_DS_BASE, %eax    
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss

    /* Clean regs */
    xorl    %eax, %eax      // accumulator
    xorl    %ebx, %ebx      // base
    xorl    %ecx, %ecx      // count
    xorl    %edx, %edx      // data
    xorl    %ebp, %ebp      // Base Pointer
    xorl    %esi, %esi      // Source Index
    xorl    %edi, %edi      // Destination Index

    call    main
    jmp     halt
END(startup32)

GLOBAL(kernel_start)
    movl    4(%esp), %eax
    movl    %eax, 1f

    .byte   0xea                        # ljmpl opcode
1:  .long   0xc1000000                  # address
    .word   GDT_ENTRY_KERNEL_CS_BASE    # segment
END(kernel_start)

GLOBAL(halt)
    cli
    hlt
END(halt)
