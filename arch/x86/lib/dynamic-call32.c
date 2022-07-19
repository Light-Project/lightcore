/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <dynamic-call.h>
#include <export.h>

long dynamic_call(dynamic_call_t fn, unsigned int num, void *args[])
{
    long retval;

    asm volatile (
        "pushl  %%ebp           \n"
        "movl   %%esp, %%ebp    \n"
        "testl  %%ecx, %%ecx    \n"
        "jz     2f              \n"
        "1:                     \n"
        "subl   $4, %%eax       \n"
        "pushl  (%%eax)         \n"
        "loop   1b              \n"
        "2:                     \n"
        "call   *%%ebx          \n"
        "leave                  \n"
        : "=a"(retval)
        : "0"(args + num), "c"(num), "b"(fn)
        : "memory"
    );

    return retval;
}
EXPORT_SYMBOL(dynamic_call);
