#ifndef _ASM_GENERIC_HEAD_H_
#define _ASM_GENERIC_HEAD_H_

#ifndef __ASSEMBLY__

#define UBOOT_NMLEN     32
#define UBOOT_MAGIC     0x27051956

struct uboot_head {
    be32        magic;              // Image Header Magic Number
    be32        hcrc;               // Image Header CRC Checksum
    be32        time;               // Image Creation Timestamp
    be32        size;               // Image Data Size
    be32        load;               // Data Load Address
    be32        ep;                 // Entry Point Address
    be32        dcrc;               // Image Data CRC Checksum
    uint8_t     os;                 // Operating System
    uint8_t     arch;               // CPU architecture
    uint8_t     type;               // Image Type
    uint8_t     comp;               // Compression Type
    uint8_t     name[UBOOT_NMLEN];  // Image Name
} __attribute__((__packed__));

struct boot_head {
    uint64_t    jump[2];        // 0x00: jump code
    uint64_t    para[2];        // 0x10: boot para
    uint8_t     magic[11];      // 0x20: kernel magic
    uint8_t     opts;           // 0x2b: options
    uint32_t    Res0;           // 0x2c: Res
    uint32_t    hsize;          // 0x30: Head size
    uint32_t    hcrc;           // 0x34: Head crc
    uint32_t    size;           // 0x38: kernel size
    uint32_t    crc;            // 0x3c: kernel crc
} __attribute__((__packed__));

extern struct boot_head boot_head;

#else

.macro  KERNEL_HEADER
    .long   0, 0, 0, 0          /* 0x10: boot parameter */
    .asciz  "lightcore!"        /* 0x20: system magic number */
    .byte   0                   /* 0x2b: options */
    .long   0                   /* 0x2c: Res */
    .long   _ld_head_size       /* 0x30: Head size */
    .long   0                   /* 0x34: Head crc32 */
    .long   _ld_image_size      /* 0x38: Kernel size */
    .long   0                   /* 0x3c: Head crc32 */
.endm

#endif /* __ASSEMBLY__ */

#endif /* _ASM_GENERIC_HEAD_H_ */
