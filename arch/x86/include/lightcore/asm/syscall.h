/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_X86_SYSCALL_H_
#define _LIGHTCORE_ASM_X86_SYSCALL_H_

static __always_inline long
syscall(unsigned int callnr, long arg1, long arg2,
        long arg3, long arg4, long arg5, long arg6)
{
    long retval;

    asm volatile (
        "pushl  %%ebp               \n"
        "pushl  %%edi               \n"
        "pushl  %%esi               \n"
        "pushl  %%edx               \n"
        "pushl  %%ecx               \n"
        "pushl  %%ebx               \n"
        "movl   0x14(%%eax), %%ebp  \n"
        "movl   0x14(%%eax), %%edi  \n"
        "movl   0x10(%%eax), %%esi  \n"
        "movl   0x0c(%%eax), %%edx  \n"
        "movl   0x08(%%eax), %%ecx  \n"
        "movl   0x04(%%eax), %%ebx  \n"
        "movl   0x00(%%eax), %%eax  \n"
        "int    $0x80               \n"
        "popl   %%ebx               \n"
        "popl   %%ecx               \n"
        "popl   %%edx               \n"
        "popl   %%esi               \n"
        "popl   %%edi               \n"
        "popl   %%ebp               \n"
        : "=a"(retval)
        : "a"(&callnr)
        : "memory"
    );

    return retval;
}

#endif  /* _LIGHTCORE_ASM_X86_SYSCALL_H_ */
