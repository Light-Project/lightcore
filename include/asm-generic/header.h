#ifndef _ASM_GENERIC_HEAD_H_ 
#define _ASM_GENERIC_HEAD_H_

#ifndef __ASSEMBLY__

typedef struct boot_head{
    uint64_t    jump[2];        // 0x00: jump code
    uint32_t    para[4];        // 0x10: boot para
    uint8_t     magic[11];      // 0x20: kernel magic
    uint8_t     opts;           // 0x2b: options
    uint32_t    Res0;           // 0x2c: Res
    uint32_t    hsize;          // 0x30: Head size
    uint32_t    hcrc;           // 0x34: Head crc
    uint32_t    size;           // 0x38: kernel size
    uint32_t    crc;            // 0x3c: kernel crc
} __packed boot_head_t;

#endif /* __ASSEMBLY__ */

#ifdef __ASSEMBLY__

.macro  KERNEL_HEADER
    .long   0, 0, 0, 0          /* 0x10: boot parameter */
    .asciz  "lightcore!"        /* 0x20: system magic number */
    .byte   0                   /* 0x2b: options */
    .long   0                   /* 0x2c: Res */
    .long   _ld_head_size       /* 0x30: Head size */
    .long   0                   /* 0x34: Head crc32 */
    .long   0                   /* 0x38: Kernel size */
    .long   0                   /* 0x3c: Head crc32 */
.endm

#endif /* __ASSEMBLY__ */

#endif /* _ASM_GENERIC_HEAD_H_ */
