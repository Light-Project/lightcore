/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BOOTPARAM_H_
#define _ASM_X86_BOOTPARAM_H_

#include <bits.h>

/* loadflags */
#define LOADED_HIGH     BIT(0)
#define KASLR_FLAG      BIT(1)
#define QUIET_FLAG      BIT(5)
#define KEEP_SEGMENTS   BIT(6)
#define CAN_USE_HEAP    BIT(7)

#ifndef __ASSEMBLY__

#include <asm/e820.h>

struct kboot_params {
    uint8_t setup_sects;
    uint16_t root_flags;
    uint32_t syssize;
    uint16_t ram_size;
    uint16_t vid_mode;
    uint16_t root_dev;
    uint16_t boot_flag;
    uint16_t jump;
    uint32_t header;
    uint16_t version;
    uint32_t realmode_swtch;
    uint16_t start_sys_seg;
    uint16_t kernel_version;
    uint8_t type_of_loader;
    uint8_t loadflags;
    uint16_t setup_move_size;
    uint32_t code32_start;
    uint32_t ramdisk_image;
    uint32_t ramdisk_size;
    uint32_t bootsect_kludge;
    uint16_t heap_end_ptr;
    uint8_t ext_loader_ver;
    uint8_t ext_loader_type;
    uint32_t cmd_line_ptr;
    uint32_t initrd_addr_max;
    uint32_t kernel_alignment;
    uint8_t relocatable_kernel;
    uint8_t min_alignment;
    uint16_t xloadflags;
    uint32_t cmdline_size;
    uint32_t hardware_subarch;
    uint64_t hardware_subarch_data;
    uint32_t payload_offset;
    uint32_t payload_length;
    uint64_t setup_data;
    uint64_t pref_address;
    uint32_t init_size;
    uint32_t handover_offset;
    uint32_t kernel_info_offset;
} __packed;

struct bootparam {
    struct kboot_params kboot_params;
    struct e820_table e820_table;
};

#include <asm-generic/header.h>

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_BOOTPARAM_H_ */
