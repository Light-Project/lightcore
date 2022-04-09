/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_MSR_H_
#define _ARCH_X86_MSR_H_

/* x86-64 specific MSRs */
#define MSR_EFER                0xc0000080  /* extended feature register */
#define MSR_STAR                0xc0000081  /* legacy mode SYSCALL target */
#define MSR_LSTAR               0xc0000082  /* long mode SYSCALL target */
#define MSR_CSTAR               0xc0000083  /* compat mode SYSCALL target */
#define MSR_SYSCALL_MASK        0xc0000084  /* EFLAGS mask for syscall */
#define MSR_FS_BASE             0xc0000100  /* 64bit FS base */
#define MSR_GS_BASE             0xc0000101  /* 64bit GS base */
#define MSR_KERNEL_GS_BASE      0xc0000102  /* SwapGS GS shadow */
#define MSR_TSC_AUX             0xc0000103  /* Auxiliary TSC */

/* EFER bits: */
#define _EFER_SCE               0           /* SYSCALL/SYSRET */
#define _EFER_LME               8           /* Long mode enable */
#define _EFER_LMA               10          /* Long mode active (read-only) */
#define _EFER_NX                11          /* No execute enable */
#define _EFER_SVME              12          /* Enable virtualization */
#define _EFER_LMSLE             13          /* Long Mode Segment Limit Enable */
#define _EFER_FFXSR             14          /* Enable Fast FXSAVE/FXRSTOR */

#define EFER_SCE                (1 << _EFER_SCE)
#define EFER_LME                (1 << _EFER_LME)
#define EFER_LMA                (1 << _EFER_LMA)
#define EFER_NX	                (1 << _EFER_NX)
#define EFER_SVME               (1 << _EFER_SVME)
#define EFER_LMSLE              (1 << _EFER_LMSLE)
#define EFER_FFXSR              (1 << _EFER_FFXSR)

#endif  /* _ARCH_X86_MSR_H_ */
