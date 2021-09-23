/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>

    .text
SYM_CODE_START(strncmp)
    /* if (len == 0) return 0 */
    cmp     a3, #0
    moveq   a1, #0
    moveq   pc, lr

    /* subtract one to have the index of the last character to check */
    sub     a3, #1

    /* ip == last src address to compare */
    add     ip, a1, a3

1:
    ldrb    v1, [a1], #1
    ldrb    v2, [a2], #1
    cmp     ip, a1
    cmpcs   v1, #1
    cmpcs   v1, v2
    beq     1b
    sub     a1, v1, v2
    mov     pc, lr
SYM_FUNC_END(strncmp)
