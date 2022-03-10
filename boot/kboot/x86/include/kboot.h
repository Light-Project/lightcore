/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <kboot16.h>
#include <lib.h>
#include <asm/header.h>

extern uint8_t sentinel[2];
extern struct kboot_params kboot_params;
extern uint8_t setup_sects;
extern uint16_t root_flags;
extern uint32_t syssize;
extern uint16_t ram_size;
extern uint16_t vid_mode;
extern uint16_t root_dev;
extern uint16_t boot_flag;
extern uint32_t header;
extern uint16_t version;
extern uint32_t realmode_swtch;
extern uint16_t start_sys_seg;
extern uint16_t kernel_version;
extern uint8_t type_of_loader;
extern uint8_t loadflags;
extern uint16_t setup_move_size;
extern uint32_t code32_start;
extern uint32_t ramdisk_image;
extern uint32_t ramdisk_size;
extern uint32_t bootsect_kludge;
extern uint16_t heap_end_ptr;
extern uint8_t ext_loader_ver;
extern uint8_t ext_loader_type;
extern uint32_t cmd_line_ptr;
extern uint32_t initrd_addr_max;
extern uint32_t kernel_alignment;
extern uint8_t min_alignment;
extern uint8_t relocatable_kernel;
extern uint16_t xloadflags;
extern uint32_t cmdline_size;
extern uint32_t hardware_subarch;
extern uint64_t hardware_subarch_data;
extern uint32_t payload_offset;
extern uint32_t payload_length;
extern uint64_t setup_data;
extern uint64_t pref_address;
extern uint32_t init_size;
extern uint32_t handover_offset;
extern uint32_t kernel_info_offset;

extern uint8_t _ld_piggy_start;
extern uint8_t _ld_piggy_end;
extern uint8_t _ld_heap_start;
extern uint8_t _ld_heap_end;

#define piggy_start ((void *)&_ld_piggy_start)
#define piggy_end   ((void *)&_ld_piggy_end)
#define piggy_size  ((size_t)piggy_end - (size_t)piggy_start)

#define heap_start  ((void *)&_ld_heap_start)
#define heap_end    ((void *)&_ld_heap_end)
#define heap_size   ((size_t)heap_end - (size_t)heap_start)

extern struct e820_table e820_table;
extern struct kboot_params kboot_params;

extern void __noreturn kernel_start(uint16_t segment, uint32_t offset);
extern void __noreturn halt(void);
extern void kernel_map(void);
extern void segment_init(void);
extern void console_print(const char *str);
extern void console_clear(void);

#endif  /* _KBOOT_H_ */
