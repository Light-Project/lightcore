/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_SEG_H_
#define _ARCH_X86_SEG_H_

/*
 * X86 GDT layout
 *
 *  0   ->  Null required by processor      <=== cacheline #1
 *  1   ->  Bootloader code segment
 *  2   ->  Bootloader data segment
 *  3   ->  Bootloader task state segment
 *
 * Start of TLS (Thread-Local Storage) segments:
 *
 *  4   ->  unused                          <=== cacheline #2
 *  5   ->  unused
 *  6   ->  TLS segment #1      [ glibc's TLS segment ]
 *  7   ->  TLS segment #2      [ Wine's %fs Win32 segment ]
 *  8   ->  TLS segment #3                  <=== cacheline #3
 *
 * Start of Reserved (For later use) segments:
 *
 *  9   ->  Reserved
 *  10  ->  Reserved
 *  11  ->  Reserved
 *
 * Start of kernel segments:
 *
 *  12  ->  Kernel code segment             <=== cacheline #4
 *  13  ->  Kernel data segment
 *  14  ->  User code segment
 *  15  ->  User data segment
 *  16  ->  Task state segment              <=== cacheline #5
 *  17  ->  Local descriptor table
 *
 *  18  ->  BIOS PNP support
 *  19  ->  BIOS PNP support
 *  20  ->  BIOS PNP support                <=== cacheline #6
 *  21  ->  BIOS APM support
 *
 *  26  ->  ESPFIX small SS
 *  27  ->  PERCPU
 *  28  ->  unused
 *  29  ->  unused
 *  30  ->  unused
 *  31  ->  TSS for double fault handler
 */

/* Use for bootloader only */

#define GDT_ENTRY_BOOT_CS                   1
#define GDT_ENTRY_BOOT_DS                   2
#define GDT_ENTRY_BOOT_TSS                  3

#define GDT_ENTRY_BOOT_CS_BASE              (GDT_ENTRY_BOOT_CS  * 8)
#define GDT_ENTRY_BOOT_DS_BASE              (GDT_ENTRY_BOOT_DS  * 8)
#define GDT_ENTRY_BOOT_TSS_BASE             (GDT_ENTRY_BOOT_TSS * 8)

#define GDT_ENTRY_KERNEL_CS                 12
#define GDT_ENTRY_KERNEL_DS                 13
#define GDT_ENTRY_USER_CS                   14
#define GDT_ENTRY_USER_DS                   15
#define GDT_ENTRY_TSS                       16
#define GDT_ENTRY_LDT                       17

#define GDT_ENTRY_KERNEL_CS_BASE            (GDT_ENTRY_KERNEL_CS * 8)
#define GDT_ENTRY_KERNEL_DS_BASE            (GDT_ENTRY_KERNEL_DS * 8)
#define GDT_ENTRY_USER_CS_BASE              (GDT_ENTRY_USER_CS   * 8)
#define GDT_ENTRY_USER_DS_BASE              (GDT_ENTRY_USER_DS   * 8)
#define GDT_ENTRY_TSS_BASE                  (GDT_ENTRY_TSS       * 8)
#define GDT_ENTRY_LDT_BASE                  (GDT_ENTRY_LDT       * 8)

#define GDT_ENTRY_PNPBIOS_CS32              18
#define GDT_ENTRY_PNPBIOS_CS16              19
#define GDT_ENTRY_PNPBIOS_DS                20
#define GDT_ENTRY_PNPBIOS_TS1               21
#define GDT_ENTRY_PNPBIOS_TS2               22
#define GDT_ENTRY_APMBIOS_CS32              23
#define GDT_ENTRY_APMBIOS_CS16              24
#define GDT_ENTRY_APMBIOS_DS                25

#define GDT_ENTRY_PNPBIOS_CS32_BASE         (GDT_ENTRY_PNPBIOS_CS32 * 8)
#define GDT_ENTRY_PNPBIOS_CS16_BASE         (GDT_ENTRY_PNPBIOS_CS16 * 8)
#define GDT_ENTRY_PNPBIOS_DS_BASE           (GDT_ENTRY_PNPBIOS_DS   * 8)
#define GDT_ENTRY_PNPBIOS_TS1_BASE          (GDT_ENTRY_PNPBIOS_TS1  * 8)
#define GDT_ENTRY_PNPBIOS_TS2_BASE          (GDT_ENTRY_PNPBIOS_TS2  * 8)
#define GDT_ENTRY_APMBIOS_BASE_BASE         (GDT_ENTRY_APMBIOS_BASE * 8)
#define GDT_ENTRY_APMBIOS_CS32_BASE         (GDT_ENTRY_APMBIOS_CS32 * 8)
#define GDT_ENTRY_APMBIOS_CS16_BASE         (GDT_ENTRY_APMBIOS_CS16 * 8)
#define GDT_ENTRY_APMBIOS_DS_BASE           (GDT_ENTRY_APMBIOS_DS   * 8)

#define GDT_ENTRY_PERCPU                    27
#define GDT_ENTRY_DF_TSS                    31

#define GDT_ENTRY_PERCPU_BASE               (GDT_ENTRY_PERCPU * 8)
#define GDT_ENTRY_DF_TSS_BASE               (GDT_ENTRY_DF_TSS * 8)

#define GDT_LENTRY_KERNEL32_CS              1
#define GDT_LENTRY_KERNEL_CS                2
#define GDT_LENTRY_KERNEL_DS                3

#define GDT_LENTRY_KERNEL32_CS_BASE         (GDT_LENTRY_KERNEL32_CS * 8)
#define GDT_LENTRY_KERNEL_CS_BASE           (GDT_LENTRY_KERNEL_CS * 8)
#define GDT_LENTRY_KERNEL_DS_BASE           (GDT_LENTRY_KERNEL_DS * 8)

#define GDT_LENTRY_DEFAULT_USER32_CS        4
#define GDT_LENTRY_DEFAULT_USER_DS          5
#define GDT_LENTRY_DEFAULT_USER_CS          6

#define GDT_LENTRY_DEFAULT_USER32_CS_BASE   (GDT_LENTRY_DEFAULT_USER32_CS * 8)
#define GDT_LENTRY_DEFAULT_USER_DS_BASE     (GDT_LENTRY_DEFAULT_USER_DS * 8)
#define GDT_LENTRY_DEFAULT_USER_CS_BASE     (GDT_LENTRY_DEFAULT_USER_CS * 8)

#ifdef CONFIG_ARCH_X86_32
#define GDT_ENTRY_MAX 64
#else
#define GDT_ENTRY_MAX 16
#endif

#endif  /* _ARCH_X86_SEG_H_ */
