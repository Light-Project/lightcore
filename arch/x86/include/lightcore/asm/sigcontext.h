/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_X86_SIGCONTEXT_H_
#define _LIGHTCORE_ASM_X86_SIGCONTEXT_H_

#include <lightcore/types.h>

#define SIGCONTEXT_GS       0
#define SIGCONTEXT_FS       1
#define SIGCONTEXT_ES       2
#define SIGCONTEXT_DS       3
#define SIGCONTEXT_EDI      4
#define SIGCONTEXT_ESI      5
#define SIGCONTEXT_EBP      6
#define SIGCONTEXT_ESP      7
#define SIGCONTEXT_EBX      8
#define SIGCONTEXT_EDX      9
#define SIGCONTEXT_ECX      10
#define SIGCONTEXT_EAX      11
#define SIGCONTEXT_TRAPNO   12
#define SIGCONTEXT_ERR      13
#define SIGCONTEXT_EIP      14
#define SIGCONTEXT_CS       15
#define SIGCONTEXT_EFL      16
#define SIGCONTEXT_UESP     17
#define SIGCONTEXT_SS       18
#define SIGCONTEXT_REGS     19

#endif /* _LIGHTCORE_ASM_X86_SIGCONTEXT_H_ */
