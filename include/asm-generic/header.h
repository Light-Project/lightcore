/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_HEAD_H_
#define _ASM_GENERIC_HEAD_H_

#define UBOOT_NMLEN     32
#define UBOOT_MAGIC     0x27051956

#ifndef __ASSEMBLY__

struct uboot_head {
    be32        magic;              /* 0x00: Image Header Magic Number  */
    be32        hcrc;               /* 0x04: Image Header CRC Checksum  */
    be32        time;               /* 0x08: Image Creation Timestamp   */
    be32        size;               /* 0x0c: Image Data Size            */
    be32        load;               /* 0x10: Data Load Address          */
    be32        ep;                 /* 0x14: Entry Point Address        */
    be32        dcrc;               /* 0x18: Image Data CRC Checksum    */
    uint8_t     os;                 /* 0x1c: Operating System           */
    uint8_t     arch;               /* 0x1d: CPU architecture           */
    uint8_t     type;               /* 0x1e: Image Type                 */
    uint8_t     comp;               /* 0x1f: Compression Type           */
    uint8_t     name[UBOOT_NMLEN];  /* 0x20: Image Name                 */
} __attribute__((__packed__));

struct boot_head {
    uint64_t    jump[2];            /* 0x00: Jump Code                  */
    uint8_t     magic[11];          /* 0x10: Kernel Magic               */
    uint8_t     opts;               /* 0x1b: Options                    */
    uint32_t    Res0;               /* 0x1c: Res                        */
    uint32_t    hsize;              /* 0x20: Head Data Size             */
    uint32_t    hcrc;               /* 0x24: Head crc                   */
    uint32_t    size;               /* 0x28: Kernel Data Size           */
    uint32_t    crc;                /* 0x2c: Kernel crc                 */
    uint64_t    cmd;                /* 0x30: Boot Command Line          */
    uint64_t    dtb;                /* 0x38: DTB Pointer                */
    uint64_t    stdout;             /* 0x40: Stdout Pointer             */
    uint64_t    params;             /* 0x48: Boot_params                */
} __attribute__((__packed__));

extern struct boot_head boot_head;

#else  /* !__ASSEMBLY__ */

.macro  KERNEL_HEADER
    .asciz  "lightcore!"            /* 0x10: Kernel Magic               */
    .byte   0                       /* 0x1b: Options                    */
    .long   0                       /* 0x1c: Res                        */
    .long   _ld_head_size           /* 0x20: Head Data Size             */
    .long   0                       /* 0x24: Head crc                   */
    .long   _ld_image_size          /* 0x28: Kernel Data Size           */
    .long   0                       /* 0x2c: Kernel crc                 */
    .long   0, 0                    /* 0x30: Boot Command Line          */
    .long   0, 0                    /* 0x38: DTB Pointer                */
    .long   0, 0                    /* 0x40: Stdout Pointer             */
    .long   0, 0                    /* 0x48: Boot_params                */
.endm

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_HEAD_H_ */
