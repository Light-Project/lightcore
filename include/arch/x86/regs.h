/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_REGS_H_
#define _ARCH_REGS_H_

#include <bits.h>

#define EFLAGS_CF       BIT(0)              /* Carry Flag */
#define EFLAGS_FIXED    BIT(1)              /* Bit 1 - always on */
#define EFLAGS_PF       BIT(2)              /* Parity Flag */
#define EFLAGS_AF       BIT(4)              /* Auxiliary carry Flag */
#define EFLAGS_ZF       BIT(6)              /* Zero Flag */
#define EFLAGS_SF       BIT(7)              /* Sign Flag */
#define EFLAGS_TF       BIT(8)              /* Trap Flag */
#define EFLAGS_IF       BIT(9)              /* Interrupt Flag */
#define EFLAGS_DF       BIT(10)             /* Direction Flag */
#define EFLAGS_OF       BIT(11)             /* Overflow Flag */
#define EFLAGS_IOPL     BIT(12)             /* I/O Privilege Level (2 bits) */
#define EFLAGS_NT       BIT(14)             /* Nested Task */
#define EFLAGS_RF       BIT(16)             /* Resume Flag */
#define EFLAGS_VM       BIT(17)             /* Virtual Mode */
#define EFLAGS_AC       BIT(18)             /* Alignment Check/Access Control */
#define EFLAGS_VIF      BIT(19)             /* Virtual Interrupt Flag */
#define EFLAGS_VIP      BIT(20)             /* Virtual Interrupt Pending */
#define EFLAGS_ID       BIT(21)             /* CPUID detection */

#define CR0_PE          BIT(0)              /* Protection Enable */
#define CR0_MP          BIT(1)              /* Monitor Coprocessor */
#define CR0_EM          BIT(2)              /* Emulation */
#define CR0_TS          BIT(3)              /* Task Switched */
#define CR0_ET          BIT(4)              /* Extension Type */
#define CR0_NE          BIT(5)              /* Numeric Error */
#define CR0_WP          BIT(16)             /* Write Protect */
#define CR0_AM          BIT(18)             /* Alignment Mask */
#define CR0_NW          BIT(29)             /* Not Write-through */
#define CR0_CD          BIT(30)             /* Cache Disable */
#define CR0_PG          BIT(31)             /* Paging */

#define CR3_PWT         BIT(3)              /* Page-level Cache Disable */
#define CR3_PCD         BIT(4)              /* Page-level Write-Through */
#define CR3_PDB         BIT_RANGE(31, 12)   /* Page-Directory Base */

#define CR4_VME         BIT(0)              /* Virtual-8086 Mode Extensions */
#define CR4_PVI         BIT(1)              /* Protected-Mode Virtual Interrupts */
#define CR4_TSD         BIT(2)              /* Time Stamp Disable */
#define CR4_DE          BIT(3)              /* Debugging Extensions */
#define CR4_PSE         BIT(4)              /* Page Size Extensions */
#define CR4_PAE         BIT(5)              /* Physical Address Extension */
#define CR4_MCE         BIT(6)              /* Machine-Check Enable */
#define CR4_PGE         BIT(7)              /* Page Global Enable */
#define CR4_PCE         BIT(8)              /* Performance-Monitoring Counter Enable */
#define CR4_OSFXSR      BIT(9)              /* Operating System Support for FXSAVE and FXRSTOR instructions */
#define CR4_OSXMMEXCPT  BIT(10)             /* Operating System Support for Unmasked SIMD Floating-Point Exceptions */
#define CR4_UMIP        BIT(11)             /* User-Mode Instruction Prevention */
#define CR4_LA57        BIT(12)             /* 57-bit linear addresses */
#define CR4_VMXE        BIT(13)             /* VMX-Enable Bit */
#define CR4_SMXE        BIT(14)             /* SMX-Enable Bit */
#define CR4_FSGSBASE    BIT(16)             /* FSGSBASE-Enable Bit */
#define CR4_PCIDE       BIT(17)             /* PCID-Enable Bit */
#define CR4_OSXSAVE     BIT(18)             /* XSAVE and Processor Extended States-Enable Bit */
#define CR4_SMEP        BIT(20)             /* SMEP-Enable Bit */
#define CR4_SMAP        BIT(21)             /* SMAP-Enable Bit */
#define CR4_PKE         BIT(22)             /* Enable protection keys for user-mode pages */
#define CR4_CTE         BIT(23)             /* Control-flow Enforcement Technology */
#define CR4_PKS         BIT(24)             /* Enable protection keys for supervisor-mode pages */

#define DR0_DEFAULT     0x00000000
#define DR1_DEFAULT     0x00000000
#define DR2_DEFAULT     0x00000000
#define DR3_DEFAULT     0x00000000
#define DR6_DEFAULT     0xffff0ff0
#define DR7_DEFAULT     0x00000400

#endif
