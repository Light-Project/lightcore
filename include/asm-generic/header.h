/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_HEAD_H_
#define _ASM_GENERIC_HEAD_H_

#define UBOOT_NMLEN     32
#define UBOOT_MAGIC     0x27051956

#ifndef __ASSEMBLY__

struct uboot_head {
    be32        magic;              /* Image Header Magic Number    */
    be32        hcrc;               /* Image Header CRC Checksum    */
    be32        time;               /* Image Creation Timestamp     */
    be32        size;               /* Image Data Size              */
    be32        load;               /* Data Load Address            */
    be32        ep;                 /* Entry Point Address          */
    be32        dcrc;               /* Image Data CRC Checksum      */
    uint8_t     os;                 /* Operating System             */
    uint8_t     arch;               /* CPU architecture             */
    uint8_t     type;               /* Image Type                   */
    uint8_t     comp;               /* Compression Type             */
    uint8_t     name[UBOOT_NMLEN];  /* Image Name                   */
} __attribute__((__packed__));

struct boot_head {
    uint64_t    jump[2];            /* 0x00: jump code      */
    uint8_t     magic[11];          /* 0x10: kernel magic   */
    uint8_t     opts;               /* 0x1b: options        */
    uint32_t    Res0;               /* 0x1c: Res            */
    uint32_t    hsize;              /* 0x20: Head size      */
    uint32_t    hcrc;               /* 0x24: Head crc       */
    uint32_t    size;               /* 0x28: kernel size    */
    uint32_t    crc;                /* 0x2c: kernel crc     */
    uint64_t    para;               /* 0x30: boot parameter */
    uint64_t    dtb;                /* 0x38: dtb pointer    */
    uint64_t    stdout;             /* 0x40: stdout pointer */
} __attribute__((__packed__));

extern struct boot_head boot_head;

#else  /* !__ASSEMBLY__ */

.macro  KERNEL_HEADER
    .asciz  "lightcore!"            /* 0x10: kernel number  */
    .byte   0                       /* 0x1b: options        */
    .long   0                       /* 0x1c: Res            */
    .long   _ld_head_size           /* 0x20: Head size      */
    .long   0                       /* 0x24: Head crc32     */
    .long   _ld_image_size          /* 0x28: Kernel size    */
    .long   0                       /* 0x2c: Head crc32     */
    .long   0, 0                    /* 0x30: boot parameter */
    .long   0, 0                    /* 0x38: dtb pointer    */
    .long   0, 0                    /* 0x40: stdout pointer */
.endm

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_HEAD_H_ */
