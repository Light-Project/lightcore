/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#define PIGGY_MAGIC     0x55aa00ff
#define EBDA_START      0x9fc00

#ifndef __ASSEMBLY__

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

extern void *_ld_piggy_start[];
extern void *_ld_piggy_end[];
extern void *_ld_heap_start[];
extern void *_ld_heap_end[];

#define __bootparam __section(".bootparam")
extern struct bootparam bootparam;

#define piggy_start ((void *)_ld_piggy_start)
#define piggy_end   ((void *)_ld_piggy_end)
#define piggy_size  ((uintptr_t)piggy_end - (uintptr_t)piggy_start)
#define piggy_load  ((void *)(uintptr_t)code32_start)

#define heap_start  ((void *)_ld_heap_start)
#define heap_end    ((void *)_ld_heap_end)
#define heap_size   ((uintptr_t)heap_end - (uintptr_t)heap_start)

extern void kboot_header(void);
extern void __noreturn kernel_start(uint16_t segment, void *offset);
extern void __noreturn halt(void);
extern void kernel_map(void);
extern void video_print(const char *str);
extern void video_clear(void);
extern void serial_print(const char *str);
extern void serial_init(void);

#endif /* __ASSEMBLY__ */
#endif /* _KBOOT_H_ */
