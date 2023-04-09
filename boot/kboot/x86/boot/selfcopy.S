/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kboot.h>
#include <arch/x86/seg.h>

#define printf(str) \
    movw    $str, %si; \
    call    realmode_printf

    .code16
    .section .startup, "ax"

SYM_FUNC_START(selfcopy)
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

    movl    %eax, %ds
    movl    %eax, %es

    pushl   %eax
    pushl   $1f
    lretl
1:

    /* Save boot_offset first */
    movl    %ebx, boot_offset

    /* Clean screen */
    movb    $0x00, %ah
    movb    $0x02, %al
    int     $0x10

/*
 * We need to turn on A20 before we load
 * the kernel into high-level memory.
 */
    printf(TXT_A20ENABLE)
    call    a20_enable
    testw   %ax, %ax
    jnz     1f

    printf(TXT_A20ERROR)
    jmp     halt_16
1:

    movl    boot_offset, %ebx
    addl    %ebx, gdt_struct + 2
    call    bigreal_init

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

    movl    %ds, %eax
    movl    %eax, %es

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
 * We need to probe memory in real mode.
 */
    movl    %ds, %eax
    movl    %eax, %es

    printf(TXT_MEMDETECT)
    call    memdetect

/*
 * Tell the BIOS what CPU mode
 * we intend to run in.
 */
#ifdef CONFIG_ARCH_X86_64
    movw    $0xec00, %ax
    movw    $0x0002, %bx
    int     $0x15
#endif

/*
 * Because of some problems with the compiler,
 * we can't use location independent, so we
 * copy the code to the absolute address.
 */
    call    bigreal_init
    movl    boot_offset, %esi
    movl    $_ld_header_start, %edi
    addl    %edi, %esi
    movl    $_ld_memory_size, %ecx
    shrl    $2, %ecx
    rep;    movsl %es:(%esi), %es:(%edi)

    /* Jump to linkage address */
    ljmpl   $0, $startup16

SYM_FUNC_END(selfcopy)

SYM_FUNC_START(realmode_printf)
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
SYM_FUNC_END(realmode_printf)

SYM_FUNC_START_LOCAL(bigreal_init)
    /* Setup GDT table */
    lgdtl   gdt_struct

    /* Enable Protected mode */
    movl    %cr0, %eax
    orl     $(1<<0), %eax
    movl    %eax, %cr0

    /* Tell 386/486 not crash */
    jmp     1f
1:

    /* Select descriptor */
    movw    $GDT_ENTRY_BOOT_DS_BASE, %ax
    movw    %ax, %es

    /* Disable pe mode */
    movl    %cr0, %eax
    xorl    $(1<<0), %eax
    movl    %eax, %cr0

    ret
SYM_FUNC_END(bigreal_init)

SYM_FUNC_START_LOCAL(halt_16)
    cli
    rep;    hlt
    jmp     halt_16
SYM_FUNC_END(halt_16)

    .section .data, "aw"
    .align 16

SYM_DATA_START_LOCAL(gdt_table)
    .long   0x00000000      /* 0x00: NULL ENTRY                     */
    .long   0x00000000      /* 0x04: NULL                           */
    .long   0x0000ffff      /* 0x08: CS ENTRY                       */
    .long   0x00cf9a00      /* 0x0c: code, read/exe, 4GB, base=0    */
    .long   0x0000ffff      /* 0x10: DS ENTRY                       */
    .long   0x00cf9200      /* 0x14: data, read/write, 4GB, base=0  */
gdt_struct:
    .word   0x0017          /* GDT Limit    */
    .long   gdt_table       /* GDT Addr     */
SYM_DATA_END(gdt_table)

    .align 4
SYM_DATA_START_LOCAL(boot_offset)
    .long   0x00000000
SYM_DATA_END(boot_offset)

    .section .rodata, "a"
    .align 4

SYM_DATA_START(boot_prompts)
TXT_BOOTBIG:    .asciz "[HEAD]: Boot head too big...\n\r"
TXT_PIGGYBIG:   .asciz "[HEAD]: Piggy data too big...\n\r"
TXT_MEMDETECT:  .asciz "[HEAD]: Detect memory...\n\r"
TXT_A20ENABLE:  .asciz "[HEAD]: Try to open a20\n\r"
TXT_A20ERROR:   .asciz "[HEAD]: Fail to open a20\n\r"
SYM_DATA_END(boot_prompts)
