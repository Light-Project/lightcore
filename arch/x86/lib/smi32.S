/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>

SYM_CODE_START(smi_trigger)
    movl    4(%esp), %eax
    pushl   %eax

    /* Save context and set register */
    xchgl   SMIREG_EDI(%eax), %edi
    xchgl   SMIREG_ESI(%eax), %esi
    xchgl   SMIREG_EBX(%eax), %ebx
    movl    SMIREG_EDX(%eax), %edx
    movl    SMIREG_ECX(%eax), %ecx
    movl    SMIREG_EAX(%eax), %eax

    /* Trigger system management interrupt */
    outb    %al, $0xb2
    xchgl   %eax, (%esp)

    /* Restore registers and save results */
    xchgl   %edi, SMIREG_EDI(%eax)
    xchgl   %esi, SMIREG_ESI(%eax)
    xchgl   %ebx, SMIREG_EBX(%eax)
    movl    %edx, SMIREG_EDX(%eax)
    movl    %ecx, SMIREG_ECX(%eax)

    popl    %edx
    movl    %edx, SMIREG_EAX(%eax)
    ret
SYM_CODE_END(smi_trigger)
