/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_CPUID_H_
#define _ARCH_X86_CPUID_H_

#define X86_CPUID_REG_EAX                               (0x00 << 8)
#define X86_CPUID_REG_EBX                               (0x01 << 8)
#define X86_CPUID_REG_ECX                               (0x02 << 8)
#define X86_CPUID_REG_EDX                               (0x03 << 8)

#define X86_CPUID_0000_0001_EDX                         (X86_CPUID_REG_EDX | (0x00000001ULL << 32)
#define X86_CPUID_8000_0001_EDX                         (X86_CPUID_REG_EDX | (0x80000001ULL << 32)
#define X86_CPUID_8086_0001_EDX                         (X86_CPUID_REG_EDX | (0x80860001ULL << 32)
#define X86_CPUID_LNX_1                                 ( | ( << 32)
#define X86_CPUID_0000_0001_ECX                         (X86_CPUID_REG_ECX | (0x00000001ULL << 32))
#define X86_CPUID_C000_0001_EDX                         (X86_CPUID_REG_EDX | (0xc0000001ULL << 32))
#define X86_CPUID_8000_0001_ECX                         (X86_CPUID_REG_ECX | (0x80000001ULL << 32))
#define X86_CPUID_LNX_2                                 ( | ( << 32)
#define X86_CPUID_LNX_3                                 ( | ( << 32)
#define X86_CPUID_0007_0000_EBX                         (X86_CPUID_REG_EBX | ( << 32)
#define X86_CPUID_000D_0001_EAX                         (X86_CPUID_REG_EAX | ( << 32)
#define X86_CPUID_LNX_4                                 ( | ( << 32)
#define X86_CPUID_0007_0001_EAX                         (X86_CPUID_REG_EAX | (0x00070001 << 32)
#define X86_CPUID_8000_0008_EBX                         (X86_CPUID_REG_EBX | (0x80000008 << 32)
#define X86_CPUID_0000_0006_EAX                         (X86_CPUID_REG_EAX | (0x00000006 << 32)
#define X86_CPUID_8000_000A_EDX                         (X86_CPUID_REG_EDX | (0x8000000a << 32)
#define X86_CPUID_0000_0007_ECX                         (X86_CPUID_REG_ECX | (0x00000007 << 32)
#define X86_CPUID_8000_0007_EBX                         (X86_CPUID_REG_EBX | (0x80000007 << 32)
#define X86_CPUID_0000_0007_EDX                         (X86_CPUID_REG_EDX | (0x00000007 << 32)
#define X86_CPUID_8000_001F_EAX                         (X86_CPUID_REG_EAX | (0x8000001f << 32)

#define X86_CPUID_FPU                                   (X86_CPUID_0000_0001_EDX |  0)      /* Onboard FPU */
#define X86_CPUID_VME                                   (X86_CPUID_0000_0001_EDX |  1)      /* Virtual Mode Extensions */
#define X86_CPUID_DE                                    (X86_CPUID_0000_0001_EDX |  2)      /* Debugging Extensions */
#define X86_CPUID_PSE                                   (X86_CPUID_0000_0001_EDX |  3)      /* Page Size Extensions */
#define X86_CPUID_TSC                                   (X86_CPUID_0000_0001_EDX |  4)      /* Time Stamp Counter */
#define X86_CPUID_MSR                                   (X86_CPUID_0000_0001_EDX |  5)      /* Model-Specific Registers */
#define X86_CPUID_PAE                                   (X86_CPUID_0000_0001_EDX |  6)      /* Physical Address Extensions */
#define X86_CPUID_MCE                                   (X86_CPUID_0000_0001_EDX |  7)      /* Machine Check Exception */
#define X86_CPUID_CX8                                   (X86_CPUID_0000_0001_EDX |  8)      /* CMPXCHG8 instruction */
#define X86_CPUID_APIC                                  (X86_CPUID_0000_0001_EDX |  9)      /* Onboard APIC */
#define X86_CPUID_SEP                                   (X86_CPUID_0000_0001_EDX | 11)      /* SYSENTER/SYSEXIT */
#define X86_CPUID_MTRR                                  (X86_CPUID_0000_0001_EDX | 12)      /* Memory Type Range Registers */
#define X86_CPUID_PGE                                   (X86_CPUID_0000_0001_EDX | 13)      /* Page Global Enable */
#define X86_CPUID_MCA                                   (X86_CPUID_0000_0001_EDX | 14)      /* Machine Check Architecture */
#define X86_CPUID_CMOV                                  (X86_CPUID_0000_0001_EDX | 15)      /* CMOV instructions (plus FCMOVcc, FCOMI with FPU) */
#define X86_CPUID_PAT                                   (X86_CPUID_0000_0001_EDX | 16)      /* Page Attribute Table */
#define X86_CPUID_PSE36                                 (X86_CPUID_0000_0001_EDX | 17)      /* 36-bit PSEs */
#define X86_CPUID_PN                                    (X86_CPUID_0000_0001_EDX | 18)      /* Processor serial number */
#define X86_CPUID_CLFLUSH                               (X86_CPUID_0000_0001_EDX | 19)      /* CLFLUSH instruction */
#define X86_CPUID_DS                                    (X86_CPUID_0000_0001_EDX | 21)      /* "dts" Debug Store */
#define X86_CPUID_ACPI                                  (X86_CPUID_0000_0001_EDX | 22)      /* ACPI via MSR */
#define X86_CPUID_MMX                                   (X86_CPUID_0000_0001_EDX | 23)      /* Multimedia Extensions */
#define X86_CPUID_FXSR                                  (X86_CPUID_0000_0001_EDX | 24)      /* FXSAVE/FXRSTOR, CR4.OSFXSR */
#define X86_CPUID_XMM                                   (X86_CPUID_0000_0001_EDX | 25)      /* "sse" */
#define X86_CPUID_XMM2                                  (X86_CPUID_0000_0001_EDX | 26)      /* "sse2" */
#define X86_CPUID_SELFSNOOP                             (X86_CPUID_0000_0001_EDX | 27)      /* "ss" CPU self snoop */
#define X86_CPUID_HT                                    (X86_CPUID_0000_0001_EDX | 28)      /* Hyper-Threading */
#define X86_CPUID_ACC                                   (X86_CPUID_0000_0001_EDX | 29)      /* "tm" Automatic clock control */
#define X86_CPUID_IA64                                  (X86_CPUID_0000_0001_EDX | 30)      /* IA-64 processor */
#define X86_CPUID_PBE                                   (X86_CPUID_0000_0001_EDX | 31)      /* Pending Break Enable */

#define X86_CPUID_SYSCALL                               (X86_CPUID_8000_0001_EDX | 11)      /* SYSCALL/SYSRET */
#define X86_CPUID_MP                                    (X86_CPUID_8000_0001_EDX | 19)      /* MP Capable */
#define X86_CPUID_NX                                    (X86_CPUID_8000_0001_EDX | 20)      /* Execute Disable */
#define X86_CPUID_MMXEXT                                (X86_CPUID_8000_0001_EDX | 22)      /* AMD MMX extensions */
#define X86_CPUID_FXSR_OPT                              (X86_CPUID_8000_0001_EDX | 25)      /* FXSAVE/FXRSTOR optimizations */
#define X86_CPUID_GBPAGES                               (X86_CPUID_8000_0001_EDX | 26)      /* "pdpe1gb" GB pages */
#define X86_CPUID_RDTSCP                                (X86_CPUID_8000_0001_EDX | 27)      /* RDTSCP */
#define X86_CPUID_LM                                    (X86_CPUID_8000_0001_EDX | 29)      /* Long Mode (x86-64, 64-bit support) */
#define X86_CPUID_3DNOWEXT                              (X86_CPUID_8000_0001_EDX | 30)      /* AMD 3DNow extensions */
#define X86_CPUID_3DNOW                                 (X86_CPUID_8000_0001_EDX | 31)      /* 3DNow */

#define X86_CPUID_RECOVERY                              (X86_CPUID_8086_0001_EDX |  0)      /* CPU in recovery mode */
#define X86_CPUID_LONGRUN                               (X86_CPUID_8086_0001_EDX |  1)      /* Longrun power control */
#define X86_CPUID_LRTI                                  (X86_CPUID_8086_0001_EDX |  3)      /* LongRun table interface */

#define X86_CPUID_CXMMX                                 ( 3 |  0)      /* Cyrix MMX extensions */
#define X86_CPUID_K6_MTRR                               ( 3 |  1)      /* AMD K6 nonstandard MTRRs */
#define X86_CPUID_CYRIX_ARR                             ( 3 |  2)      /* Cyrix ARRs (= MTRRs) */
#define X86_CPUID_CENTAUR_MCR                           ( 3 |  3)      /* Centaur MCRs (= MTRRs) */
#define X86_CPUID_K8                                    ( 3 |  4)      /* "" Opteron, Athlon64 */
#define X86_CPUID_P3                                    ( 3 |  6)      /* "" P3 */
#define X86_CPUID_P4                                    ( 3 |  7)      /* "" P4 */
#define X86_CPUID_CONSTANT_TSC                          ( 3 |  8)      /* TSC ticks at a constant rate */
#define X86_CPUID_UP                                    ( 3 |  9)      /* SMP kernel running on UP */
#define X86_CPUID_ART                                   ( 3 | 10)      /* Always running timer (ART) */
#define X86_CPUID_ARCH_PERFMON                          ( 3 | 11)      /* Intel Architectural PerfMon */
#define X86_CPUID_PEBS                                  ( 3 | 12)      /* Precise-Event Based Sampling */
#define X86_CPUID_BTS                                   ( 3 | 13)      /* Branch Trace Store */
#define X86_CPUID_SYSCALL32                             ( 3 | 14)      /* "" syscall in IA32 userspace */
#define X86_CPUID_SYSENTER32                            ( 3 | 15)      /* "" sysenter in IA32 userspace */
#define X86_CPUID_REP_GOOD                              ( 3 | 16)      /* REP microcode works well */
#define X86_CPUID_LFENCE_RDTSC                          ( 3 | 18)      /* "" LFENCE synchronizes RDTSC */
#define X86_CPUID_ACC_POWER                             ( 3 | 19)      /* AMD Accumulated Power Mechanism */
#define X86_CPUID_NOPL                                  ( 3 | 20)      /* The NOPL (0F 1F) instructions */
#define X86_CPUID_ALWAYS                                ( 3 | 21)      /* "" Always-present feature */
#define X86_CPUID_XTOPOLOGY                             ( 3 | 22)      /* CPU topology enum extensions */
#define X86_CPUID_TSC_RELIABLE                          ( 3 | 23)      /* TSC is known to be reliable */
#define X86_CPUID_NONSTOP_TSC                           ( 3 | 24)      /* TSC does not stop in C states */
#define X86_CPUID_CPUID                                 ( 3 | 25)      /* CPU has CPUID instruction itself */
#define X86_CPUID_EXTD_APICID                           ( 3 | 26)      /* Extended APICID (8 bits) */
#define X86_CPUID_AMD_DCM                               ( 3 | 27)      /* AMD multi-node processor */
#define X86_CPUID_APERFMPERF                            ( 3 | 28)      /* P-State hardware coordination feedback capability (APERF/MPERF MSRs) */
#define X86_CPUID_RAPL                                  ( 3 | 29)      /* AMD/Hygon RAPL interface */
#define X86_CPUID_NONSTOP_TSC_S3                        ( 3 | 30)      /* TSC doesn't stop in S3 state */
#define X86_CPUID_TSC_KNOWN_FREQ                        ( 3 | 31)      /* TSC has known frequency */

#define X86_CPUID_XMM3                                  (X86_CPUID_0000_0001_ECX |  0)      /* "pni" SSE-3 */
#define X86_CPUID_PCLMULQDQ                             (X86_CPUID_0000_0001_ECX |  1)      /* PCLMULQDQ instruction */
#define X86_CPUID_DTES64                                (X86_CPUID_0000_0001_ECX |  2)      /* 64-bit Debug Store */
#define X86_CPUID_MWAIT                                 (X86_CPUID_0000_0001_ECX |  3)      /* "monitor" MONITOR/MWAIT support */
#define X86_CPUID_DSCPL                                 (X86_CPUID_0000_0001_ECX |  4)      /* "ds_cpl" CPL-qualified (filtered) Debug Store */
#define X86_CPUID_VMX                                   (X86_CPUID_0000_0001_ECX |  5)      /* Hardware virtualization */
#define X86_CPUID_SMX                                   (X86_CPUID_0000_0001_ECX |  6)      /* Safer Mode eXtensions */
#define X86_CPUID_EST                                   (X86_CPUID_0000_0001_ECX |  7)      /* Enhanced SpeedStep */
#define X86_CPUID_TM2                                   (X86_CPUID_0000_0001_ECX |  8)      /* Thermal Monitor 2 */
#define X86_CPUID_SSSE3                                 (X86_CPUID_0000_0001_ECX |  9)      /* Supplemental SSE-3 */
#define X86_CPUID_CID                                   (X86_CPUID_0000_0001_ECX | 10)      /* Context ID */
#define X86_CPUID_SDBG                                  (X86_CPUID_0000_0001_ECX | 11)      /* Silicon Debug */
#define X86_CPUID_FMA                                   (X86_CPUID_0000_0001_ECX | 12)      /* Fused multiply-add */
#define X86_CPUID_CX16                                  (X86_CPUID_0000_0001_ECX | 13)      /* CMPXCHG16B instruction */
#define X86_CPUID_XTPR                                  (X86_CPUID_0000_0001_ECX | 14)      /* Send Task Priority Messages */
#define X86_CPUID_PDCM                                  (X86_CPUID_0000_0001_ECX | 15)      /* Perf/Debug Capabilities MSR */
#define X86_CPUID_PCID                                  (X86_CPUID_0000_0001_ECX | 17)      /* Process Context Identifiers */
#define X86_CPUID_DCA                                   (X86_CPUID_0000_0001_ECX | 18)      /* Direct Cache Access */
#define X86_CPUID_XMM4_1                                (X86_CPUID_0000_0001_ECX | 19)      /* "sse4_1" SSE-4.1 */
#define X86_CPUID_XMM4_2                                (X86_CPUID_0000_0001_ECX | 20)      /* "sse4_2" SSE-4.2 */
#define X86_CPUID_X2APIC                                (X86_CPUID_0000_0001_ECX | 21)      /* X2APIC */
#define X86_CPUID_MOVBE                                 (X86_CPUID_0000_0001_ECX | 22)      /* MOVBE instruction */
#define X86_CPUID_POPCNT                                (X86_CPUID_0000_0001_ECX | 23)      /* POPCNT instruction */
#define X86_CPUID_TSC_DEADLINE_TIMER                    (X86_CPUID_0000_0001_ECX | 24)      /* TSC deadline timer */
#define X86_CPUID_AES                                   (X86_CPUID_0000_0001_ECX | 25)      /* AES instructions */
#define X86_CPUID_XSAVE                                 (X86_CPUID_0000_0001_ECX | 26)      /* XSAVE/XRSTOR/XSETBV/XGETBV instructions */
#define X86_CPUID_OSXSAVE                               (X86_CPUID_0000_0001_ECX | 27)      /* "" XSAVE instruction enabled in the OS */
#define X86_CPUID_AVX                                   (X86_CPUID_0000_0001_ECX | 28)      /* Advanced Vector Extensions */
#define X86_CPUID_F16C                                  (X86_CPUID_0000_0001_ECX | 29)      /* 16-bit FP conversions */
#define X86_CPUID_RDRAND                                (X86_CPUID_0000_0001_ECX | 30)      /* RDRAND instruction */
#define X86_CPUID_HYPERVISOR                            (X86_CPUID_0000_0001_ECX | 31)      /* Running on a hypervisor */

#define X86_CPUID_XSTORE                                (X86_CPUID_C000_0001_EDX |  2)      /* "rng" RNG present (xstore) */
#define X86_CPUID_XSTORE_EN                             (X86_CPUID_C000_0001_EDX |  3)      /* "rng_en" RNG enabled */
#define X86_CPUID_XCRYPT                                (X86_CPUID_C000_0001_EDX |  6)      /* "ace" on-CPU crypto (xcrypt) */
#define X86_CPUID_XCRYPT_EN                             (X86_CPUID_C000_0001_EDX |  7)      /* "ace_en" on-CPU crypto enabled */
#define X86_CPUID_ACE2                                  (X86_CPUID_C000_0001_EDX |  8)      /* Advanced Cryptography Engine v2 */
#define X86_CPUID_ACE2_EN                               (X86_CPUID_C000_0001_EDX |  9)      /* ACE v2 enabled */
#define X86_CPUID_PHE                                   (X86_CPUID_C000_0001_EDX | 10)      /* PadLock Hash Engine */
#define X86_CPUID_PHE_EN                                (X86_CPUID_C000_0001_EDX | 11)      /* PHE enabled */
#define X86_CPUID_PMM                                   (X86_CPUID_C000_0001_EDX | 12)      /* PadLock Montgomery Multiplier */
#define X86_CPUID_PMM_EN                                (X86_CPUID_C000_0001_EDX | 13)      /* PMM enabled */

#define X86_CPUID_LAHF_LM                               (X86_CPUID_8000_0001_ECX |  0)      /* LAHF/SAHF in long mode */
#define X86_CPUID_CMP_LEGACY                            (X86_CPUID_8000_0001_ECX |  1)      /* If yes HyperThreading not valid */
#define X86_CPUID_SVM                                   (X86_CPUID_8000_0001_ECX |  2)      /* Secure Virtual Machine */
#define X86_CPUID_EXTAPIC                               (X86_CPUID_8000_0001_ECX |  3)      /* Extended APIC space */
#define X86_CPUID_CR8_LEGACY                            (X86_CPUID_8000_0001_ECX |  4)      /* CR8 in 32-bit mode */
#define X86_CPUID_ABM                                   (X86_CPUID_8000_0001_ECX |  5)      /* Advanced bit manipulation */
#define X86_CPUID_SSE4A                                 (X86_CPUID_8000_0001_ECX |  6)      /* SSE-4A */
#define X86_CPUID_MISALIGNSSE                           (X86_CPUID_8000_0001_ECX |  7)      /* Misaligned SSE mode */
#define X86_CPUID_3DNOWPREFETCH                         (X86_CPUID_8000_0001_ECX |  8)      /* 3DNow prefetch instructions */
#define X86_CPUID_OSVW                                  (X86_CPUID_8000_0001_ECX |  9)      /* OS Visible Workaround */
#define X86_CPUID_IBS                                   (X86_CPUID_8000_0001_ECX | 10)      /* Instruction Based Sampling */
#define X86_CPUID_XOP                                   (X86_CPUID_8000_0001_ECX | 11)      /* extended AVX instructions */
#define X86_CPUID_SKINIT                                (X86_CPUID_8000_0001_ECX | 12)      /* SKINIT/STGI instructions */
#define X86_CPUID_WDT                                   (X86_CPUID_8000_0001_ECX | 13)      /* Watchdog timer */
#define X86_CPUID_LWP                                   (X86_CPUID_8000_0001_ECX | 15)      /* Light Weight Profiling */
#define X86_CPUID_FMA4                                  (X86_CPUID_8000_0001_ECX | 16)      /* 4 operands MAC instructions */
#define X86_CPUID_TCE                                   (X86_CPUID_8000_0001_ECX | 17)      /* Translation Cache Extension */
#define X86_CPUID_NODEID_MSR                            (X86_CPUID_8000_0001_ECX | 19)      /* NodeId MSR */
#define X86_CPUID_TBM                                   (X86_CPUID_8000_0001_ECX | 21)      /* Trailing Bit Manipulations */
#define X86_CPUID_TOPOEXT                               (X86_CPUID_8000_0001_ECX | 22)      /* Topology extensions CPUID leafs */
#define X86_CPUID_PERFCTR_CORE                          (X86_CPUID_8000_0001_ECX | 23)      /* Core performance counter extensions */
#define X86_CPUID_PERFCTR_NB                            (X86_CPUID_8000_0001_ECX | 24)      /* NB performance counter extensions */
#define X86_CPUID_BPEXT                                 (X86_CPUID_8000_0001_ECX | 26)      /* Data breakpoint extension */
#define X86_CPUID_PTSC                                  (X86_CPUID_8000_0001_ECX | 27)      /* Performance time-stamp counter */
#define X86_CPUID_PERFCTR_LLC                           (X86_CPUID_8000_0001_ECX | 28)      /* Last Level Cache performance counter extensions */
#define X86_CPUID_MWAITX                                (X86_CPUID_8000_0001_ECX | 29)      /* MWAIT extension (MONITORX/MWAITX instructions) */

#define X86_CPUID_RING3MWAIT                            ( 7 |  0)      /* Ring 3 MONITOR/MWAIT instructions */
#define X86_CPUID_CPUID_FAULT                           ( 7 |  1)      /* Intel CPUID faulting */
#define X86_CPUID_CPB                                   ( 7 |  2)      /* AMD Core Performance Boost */
#define X86_CPUID_EPB                                   ( 7 |  3)      /* IA32_ENERGY_PERF_BIAS support */
#define X86_CPUID_CAT_L3                                ( 7 |  4)      /* Cache Allocation Technology L3 */
#define X86_CPUID_CAT_L2                                ( 7 |  5)      /* Cache Allocation Technology L2 */
#define X86_CPUID_CDP_L3                                ( 7 |  6)      /* Code and Data Prioritization L3 */
#define X86_CPUID_INVPCID_SINGLE                        ( 7 |  7)      /* Effectively INVPCID && CR4.PCIDE=1 */
#define X86_CPUID_HW_PSTATE                             ( 7 |  8)      /* AMD HW-PState */
#define X86_CPUID_PROC_FEEDBACK                         ( 7 |  9)      /* AMD ProcFeedbackInterface */
#define X86_CPUID_PTI                                   ( 7 | 11)      /* Kernel Page Table Isolation enabled */
#define X86_CPUID_RETPOLINE                             ( 7 | 12)      /* "" Generic Retpoline mitigation for Spectre variant 2 */
#define X86_CPUID_RETPOLINE_AMD                         ( 7 | 13)      /* "" AMD Retpoline mitigation for Spectre variant 2 */
#define X86_CPUID_INTEL_PPIN                            ( 7 | 14)      /* Intel Processor Inventory Number */
#define X86_CPUID_CDP_L2                                ( 7 | 15)      /* Code and Data Prioritization L2 */
#define X86_CPUID_MSR_SPEC_CTRL                         ( 7 | 16)      /* "" MSR SPEC_CTRL is implemented */
#define X86_CPUID_SSBD                                  ( 7 | 17)      /* Speculative Store Bypass Disable */
#define X86_CPUID_MBA                                   ( 7 | 18)      /* Memory Bandwidth Allocation */
#define X86_CPUID_RSB_CTXSW                             ( 7 | 19)      /* "" Fill RSB on context switches */
#define X86_CPUID_USE_IBPB                              ( 7 | 21)      /* "" Indirect Branch Prediction Barrier enabled */
#define X86_CPUID_USE_IBRS_FW                           ( 7 | 22)      /* "" Use IBRS during runtime firmware calls */
#define X86_CPUID_SPEC_STORE_BYPASS_DISABLE             ( 7 | 23)      /* "" Disable Speculative Store Bypass. */
#define X86_CPUID_LS_CFG_SSBD                           ( 7 | 24)      /* "" AMD SSBD implementation via LS_CFG MSR */
#define X86_CPUID_IBRS                                  ( 7 | 25)      /* Indirect Branch Restricted Speculation */
#define X86_CPUID_IBPB                                  ( 7 | 26)      /* Indirect Branch Prediction Barrier */
#define X86_CPUID_STIBP                                 ( 7 | 27)      /* Single Thread Indirect Branch Predictors */
#define X86_CPUID_ZEN                                   ( 7 | 28)      /* "" CPU is AMD family 0x17 or above (Zen) */
#define X86_CPUID_L1TF_PTEINV                           ( 7 | 29)      /* "" L1TF workaround PTE inversion */
#define X86_CPUID_IBRS_ENHANCED                         ( 7 | 30)      /* Enhanced IBRS */
#define X86_CPUID_MSR_IA32_FEAT_CTL                     ( 7 | 31)      /* "" MSR IA32_FEAT_CTL configured */
#define X86_CPUID_TPR_SHADOW                            ( 8 |  0)      /* Intel TPR Shadow */
#define X86_CPUID_VNMI                                  ( 8 |  1)      /* Intel Virtual NMI */
#define X86_CPUID_FLEXPRIORITY                          ( 8 |  2)      /* Intel FlexPriority */
#define X86_CPUID_EPT                                   ( 8 |  3)      /* Intel Extended Page Table */
#define X86_CPUID_VPID                                  ( 8 |  4)      /* Intel Virtual Processor ID */
#define X86_CPUID_VMMCALL                               ( 8 | 15)      /* Prefer VMMCALL to VMCALL */
#define X86_CPUID_XENPV                                 ( 8 | 16)      /* "" Xen paravirtual guest */
#define X86_CPUID_EPT_AD                                ( 8 | 17)      /* Intel Extended Page Table access-dirty bit */
#define X86_CPUID_VMCALL                                ( 8 | 18)      /* "" Hypervisor supports the VMCALL instruction */
#define X86_CPUID_VMW_VMMCALL                           ( 8 | 19)      /* "" VMware prefers VMMCALL hypercall instruction */
#define X86_CPUID_PVUNLOCK                              ( 8 | 20)      /* "" PV unlock function */
#define X86_CPUID_VCPUPREEMPT                           ( 8 | 21)      /* "" PV vcpu_is_preempted function */
#define X86_CPUID_FSGSBASE                              ( 9 |  0)      /* RDFSBASE, WRFSBASE, RDGSBASE, WRGSBASE instructions*/
#define X86_CPUID_TSC_ADJUST                            ( 9 |  1)      /* TSC adjustment MSR 0x3B */
#define X86_CPUID_SGX                                   ( 9 |  2)      /* Software Guard Extensions */
#define X86_CPUID_BMI1                                  ( 9 |  3)      /* 1st group bit manipulation extensions */
#define X86_CPUID_HLE                                   ( 9 |  4)      /* Hardware Lock Elision */
#define X86_CPUID_AVX2                                  ( 9 |  5)      /* AVX2 instructions */
#define X86_CPUID_FDP_EXCPTN_ONLY                       ( 9 |  6)      /* "" FPU data pointer updated only on x87 exceptions */
#define X86_CPUID_SMEP                                  ( 9 |  7)      /* Supervisor Mode Execution Protection */
#define X86_CPUID_BMI2                                  ( 9 |  8)      /* 2nd group bit manipulation extensions */
#define X86_CPUID_ERMS                                  ( 9 |  9)      /* Enhanced REP MOVSB/STOSB instructions */
#define X86_CPUID_INVPCID                               ( 9 | 10)      /* Invalidate Processor Context ID */
#define X86_CPUID_RTM                                   ( 9 | 11)      /* Restricted Transactional Memory */
#define X86_CPUID_CQM                                   ( 9 | 12)      /* Cache QoS Monitoring */
#define X86_CPUID_ZERO_FCS_FDS                          ( 9 | 13)      /* "" Zero out FPU CS and FPU DS */
#define X86_CPUID_MPX                                   ( 9 | 14)      /* Memory Protection Extension */
#define X86_CPUID_RDT_A                                 ( 9 | 15)      /* Resource Director Technology Allocation */
#define X86_CPUID_AVX512F                               ( 9 | 16)      /* AVX-512 Foundation */
#define X86_CPUID_AVX512DQ                              ( 9 | 17)      /* AVX-512 DQ (Double/Quad granular) Instructions */
#define X86_CPUID_RDSEED                                ( 9 | 18)      /* RDSEED instruction */
#define X86_CPUID_ADX                                   ( 9 | 19)      /* ADCX and ADOX instructions */
#define X86_CPUID_SMAP                                  ( 9 | 20)      /* Supervisor Mode Access Prevention */
#define X86_CPUID_AVX512IFMA                            ( 9 | 21)      /* AVX-512 Integer Fused Multiply-Add instructions */
#define X86_CPUID_CLFLUSHOPT                            ( 9 | 23)      /* CLFLUSHOPT instruction */
#define X86_CPUID_CLWB                                  ( 9 | 24)      /* CLWB instruction */
#define X86_CPUID_INTEL_PT                              ( 9 | 25)      /* Intel Processor Trace */
#define X86_CPUID_AVX512PF                              ( 9 | 26)      /* AVX-512 Prefetch */
#define X86_CPUID_AVX512ER                              ( 9 | 27)      /* AVX-512 Exponential and Reciprocal */
#define X86_CPUID_AVX512CD                              ( 9 | 28)      /* AVX-512 Conflict Detection */
#define X86_CPUID_SHA_NI                                ( 9 | 29)      /* SHA1/SHA256 Instruction Extensions */
#define X86_CPUID_AVX512BW                              ( 9 | 30)      /* AVX-512 BW (Byte/Word granular) Instructions */
#define X86_CPUID_AVX512VL                              ( 9 | 31)      /* AVX-512 VL (128/256 Vector Length) Extensions */
#define X86_CPUID_XSAVEOPT                              (10 |  0)      /* XSAVEOPT instruction */
#define X86_CPUID_XSAVEC                                (10 |  1)      /* XSAVEC instruction */
#define X86_CPUID_XGETBV1                               (10 |  2)      /* XGETBV with ECX = 1 instruction */
#define X86_CPUID_XSAVES                                (10 |  3)      /* XSAVES/XRSTORS instructions */
#define X86_CPUID_XFD                                   (10 |  4)      /* "" eXtended Feature Disabling */
#define X86_CPUID_CQM_LLC                               (11 |  0)      /* LLC QoS if 1 */
#define X86_CPUID_CQM_OCCUP_LLC                         (11 |  1)      /* LLC occupancy monitoring */
#define X86_CPUID_CQM_MBM_TOTAL                         (11 |  2)      /* LLC Total MBM monitoring */
#define X86_CPUID_CQM_MBM_LOCAL                         (11 |  3)      /* LLC Local MBM monitoring */
#define X86_CPUID_FENCE_SWAPGS_USER                     (11 |  4)      /* "" LFENCE in user entry SWAPGS path */
#define X86_CPUID_FENCE_SWAPGS_KERNEL                   (11 |  5)      /* "" LFENCE in kernel entry SWAPGS path */
#define X86_CPUID_SPLIT_LOCK_DETECT                     (11 |  6)      /* #AC for split lock */
#define X86_CPUID_PER_THREAD_MBA                        (11 |  7)      /* "" Per-thread Memory Bandwidth Allocation */
#define X86_CPUID_SGX1                                  (11 |  8)      /* "" Basic SGX */
#define X86_CPUID_SGX2                                  (11 |  9)      /* "" SGX Enclave Dynamic Memory Management (EDMM) */
#define X86_CPUID_AVX_VNNI                              (12 |  4)      /* AVX VNNI instructions */
#define X86_CPUID_AVX512_BF16                           (12 |  5)      /* AVX512 BFLOAT16 instructions */
#define X86_CPUID_AMX_TILE                              (18 | 24)      /* AMX tile Support */
#define X86_CPUID_CLZERO                                (13 |  0)      /* CLZERO instruction */
#define X86_CPUID_IRPERF                                (13 |  1)      /* Instructions Retired Count */
#define X86_CPUID_XSAVEERPTR                            (13 |  2)      /* Always save/restore FP error pointers */
#define X86_CPUID_RDPRU                                 (13 |  4)      /* Read processor register at user level */
#define X86_CPUID_WBNOINVD                              (13 |  9)      /* WBNOINVD instruction */
#define X86_CPUID_AMD_IBPB                              (13 | 12)      /* "" Indirect Branch Prediction Barrier */
#define X86_CPUID_AMD_IBRS                              (13 | 14)      /* "" Indirect Branch Restricted Speculation */
#define X86_CPUID_AMD_STIBP                             (13 | 15)      /* "" Single Thread Indirect Branch Predictors */
#define X86_CPUID_AMD_STIBP_ALWAYS_ON                   (13 | 17)      /* "" Single Thread Indirect Branch Predictors always-on preferred */
#define X86_CPUID_AMD_PPIN                              (13 | 23)      /* Protected Processor Inventory Number */
#define X86_CPUID_AMD_SSBD                              (13 | 24)      /* "" Speculative Store Bypass Disable */
#define X86_CPUID_VIRT_SSBD                             (13 | 25)      /* Virtualized Speculative Store Bypass Disable */
#define X86_CPUID_AMD_SSB_NO                            (13 | 26)      /* "" Speculative Store Bypass is fixed in hardware. */
#define X86_CPUID_DTHERM                                (14 |  0)      /* Digital Thermal Sensor */
#define X86_CPUID_IDA                                   (14 |  1)      /* Intel Dynamic Acceleration */
#define X86_CPUID_ARAT                                  (14 |  2)      /* Always Running APIC Timer */
#define X86_CPUID_PLN                                   (14 |  4)      /* Intel Power Limit Notification */
#define X86_CPUID_PTS                                   (14 |  6)      /* Intel Package Thermal Status */
#define X86_CPUID_HWP                                   (14 |  7)      /* Intel Hardware P-states */
#define X86_CPUID_HWP_NOTIFY                            (14 |  8)      /* HWP Notification */
#define X86_CPUID_HWP_ACT_WINDOW                        (14 |  9)      /* HWP Activity Window */
#define X86_CPUID_HWP_EPP                               (14 | 10)      /* HWP Energy Perf. Preference */
#define X86_CPUID_HWP_PKG_REQ                           (14 | 11)      /* HWP Package Level Request */
#define X86_CPUID_NPT                                   (15 |  0)      /* Nested Page Table support */
#define X86_CPUID_LBRV                                  (15 |  1)      /* LBR Virtualization support */
#define X86_CPUID_SVML                                  (15 |  2)      /* "svm_lock" SVM locking MSR */
#define X86_CPUID_NRIPS                                 (15 |  3)      /* "nrip_save" SVM next_rip save */
#define X86_CPUID_TSCRATEMSR                            (15 |  4)      /* "tsc_scale" TSC scaling support */
#define X86_CPUID_VMCBCLEAN                             (15 |  5)      /* "vmcb_clean" VMCB clean bits support */
#define X86_CPUID_FLUSHBYASID                           (15 |  6)      /* flush-by-ASID support */
#define X86_CPUID_DECODEASSISTS                         (15 |  7)      /* Decode Assists support */
#define X86_CPUID_PAUSEFILTER                           (15 | 10)      /* filtered pause intercept */
#define X86_CPUID_PFTHRESHOLD                           (15 | 12)      /* pause filter threshold */
#define X86_CPUID_AVIC                                  (15 | 13)      /* Virtual Interrupt Controller */
#define X86_CPUID_V_VMSAVE_VMLOAD                       (15 | 15)      /* Virtual VMSAVE VMLOAD */
#define X86_CPUID_VGIF                                  (15 | 16)      /* Virtual GIF */
#define X86_CPUID_V_SPEC_CTRL                           (15 | 20)      /* Virtual SPEC_CTRL */
#define X86_CPUID_SVME_ADDR_CHK                         (15 | 28)      /* "" SVME addr check */
#define X86_CPUID_AVX512VBMI                            (16 |  1)      /* AVX512 Vector Bit Manipulation instructions*/
#define X86_CPUID_UMIP                                  (16 |  2)      /* User Mode Instruction Protection */
#define X86_CPUID_PKU                                   (16 |  3)      /* Protection Keys for Userspace */
#define X86_CPUID_OSPKE                                 (16 |  4)      /* OS Protection Keys Enable */
#define X86_CPUID_WAITPKG                               (16 |  5)      /* UMONITOR/UMWAIT/TPAUSE Instructions */
#define X86_CPUID_AVX512_VBMI2                          (16 |  6)      /* Additional AVX512 Vector Bit Manipulation Instructions */
#define X86_CPUID_GFNI                                  (16 |  8)      /* Galois Field New Instructions */
#define X86_CPUID_VAES                                  (16 |  9)      /* Vector AES */
#define X86_CPUID_VPCLMULQDQ                            (16 | 10)      /* Carry-Less Multiplication Double Quadword */
#define X86_CPUID_AVX512_VNNI                           (16 | 11)      /* Vector Neural Network Instructions */
#define X86_CPUID_AVX512_BITALG                         (16 | 12)      /* Support for VPOPCNT[B,W] and VPSHUF-BITQMB instructions */
#define X86_CPUID_TME                                   (16 | 13)      /* Intel Total Memory Encryption */
#define X86_CPUID_AVX512_VPOPCNTDQ                      (16 | 14)      /* POPCNT for vectors of DW/QW */
#define X86_CPUID_LA57                                  (16 | 16)      /* 5-level page tables */
#define X86_CPUID_RDPID                                 (16 | 22)      /* RDPID instruction */
#define X86_CPUID_BUS_LOCK_DETECT                       (16 | 24)      /* Bus Lock detect */
#define X86_CPUID_CLDEMOTE                              (16 | 25)      /* CLDEMOTE instruction */
#define X86_CPUID_MOVDIRI                               (16 | 27)      /* MOVDIRI instruction */
#define X86_CPUID_MOVDIR64B                             (16 | 28)      /* MOVDIR64B instruction */
#define X86_CPUID_ENQCMD                                (16 | 29)      /* ENQCMD and ENQCMDS instructions */
#define X86_CPUID_SGX_LC                                (16 | 30)      /* Software Guard Extensions Launch Control */
#define X86_CPUID_OVERFLOW_RECOV                        (17 |  0)      /* MCA overflow recovery support */
#define X86_CPUID_SUCCOR                                (17 |  1)      /* Uncorrectable error containment and recovery */
#define X86_CPUID_SMCA                                  (17 |  3)      /* Scalable MCA */
#define X86_CPUID_AVX512_4VNNIW                         (18 |  2)      /* AVX-512 Neural Network Instructions */
#define X86_CPUID_AVX512_4FMAPS                         (18 |  3)      /* AVX-512 Multiply Accumulation Single precision */
#define X86_CPUID_FSRM                                  (18 |  4)      /* Fast Short Rep Mov */
#define X86_CPUID_AVX512_VP2INTERSECT                   (18 |  8)      /* AVX-512 Intersect for D/Q */
#define X86_CPUID_SRBDS_CTRL                            (18 |  9)      /* "" SRBDS mitigation MSR available */
#define X86_CPUID_MD_CLEAR                              (18 | 10)      /* VERW clears CPU buffers */
#define X86_CPUID_RTM_ALWAYS_ABORT                      (18 | 11)      /* "" RTM transaction always aborts */
#define X86_CPUID_TSX_FORCE_ABORT                       (18 | 13)      /* "" TSX_FORCE_ABORT */
#define X86_CPUID_SERIALIZE                             (18 | 14)      /* SERIALIZE instruction */
#define X86_CPUID_HYBRID_CPU                            (18 | 15)      /* "" This part has CPUs of more than one type */
#define X86_CPUID_TSXLDTRK                              (18 | 16)      /* TSX Suspend Load Address Tracking */
#define X86_CPUID_PCONFIG                               (18 | 18)      /* Intel PCONFIG */
#define X86_CPUID_ARCH_LBR                              (18 | 19)      /* Intel ARCH LBR */
#define X86_CPUID_AVX512_FP16                           (18 | 23)      /* AVX512 FP16 */
#define X86_CPUID_SPEC_CTRL                             (18 | 26)      /* "" Speculation Control (IBRS + IBPB) */
#define X86_CPUID_INTEL_STIBP                           (18 | 27)      /* "" Single Thread Indirect Branch Predictors */
#define X86_CPUID_FLUSH_L1D                             (18 | 28)      /* Flush L1D cache */
#define X86_CPUID_ARCH_CAPABILITIES                     (18 | 29)      /* IA32_ARCH_CAPABILITIES MSR (Intel) */
#define X86_CPUID_CORE_CAPABILITIES                     (18 | 30)      /* "" IA32_CORE_CAPABILITIES MSR */
#define X86_CPUID_SPEC_CTRL_SSBD                        (18 | 31)      /* "" Speculative Store Bypass Disable */
#define X86_CPUID_SME                                   (19 |  0)      /* AMD Secure Memory Encryption */
#define X86_CPUID_SEV                                   (19 |  1)      /* AMD Secure Encrypted Virtualization */
#define X86_CPUID_VM_PAGE_FLUSH                         (19 |  2)      /* "" VM Page Flush MSR is supported */
#define X86_CPUID_SEV_ES                                (19 |  3)      /* AMD Secure Encrypted Virtualization - Encrypted State */
#define X86_CPUID_SME_COHERENT                          (19 | 10)      /* "" AMD hardware-enforced cache coherency */

#define X86_CPUID_BIT_GET(code)                         ((code >> 0) & 0xff)
#define X86_CPUID_REG_GET(code)                         ((code >> 8) & 0xff)
#define X86_CPUID_OP_GET(code)                          ((code >> 32) & 0xffffffff)

#endif /* _ARCH_X86_CPUID_H_ */
