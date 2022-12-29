/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>

SYM_CODE_START(dynamic_call)
    pushl   %ebp
    movl    %esp, %ebp

    /* get the parameter number */
    movl    12(%ebp), %ecx
    movl    16(%ebp), %eax
    jecxz   2f

1:
    /* push all the parameters */
    pushl   -4(%eax, %ecx, 4)
    loop    1b

2:
    call    *8(%ebp)
    leave
    ret
SYM_CODE_END(dynamic_call)
