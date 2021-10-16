/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_KERNEL_LDS_H_
#define _ASM_GENERIC_KERNEL_LDS_H_

#ifndef LOAD_OFFSET
#define LOAD_OFFSET 0
#endif

/*
 * Only some architectures want to have the .notes segment visible in
 * a separate PT_NOTE ELF Program Header. When this happens, it needs
 * to be visible in both the kernel text's PT_LOAD and the PT_NOTE
 * Program Headers. In this case, though, the PT_LOAD needs to be made
 * the default again so that all the following sections don't also end
 * up in the PT_NOTE Program Header.
 */
#ifdef EMITS_PT_NOTE
#define NOTES_HEADERS		:text :note
#define NOTES_HEADERS_RESTORE	__restore_ph : { *(.__restore_ph) } :text
#else
#define NOTES_HEADERS
#define NOTES_HEADERS_RESTORE
#endif

/*
 * LD_DEAD_CODE_DATA_ELIMINATION option enables -fdata-sections, which
 * generates .data.identifier sections, which need to be pulled in with
 * .data. We don't want to pull in .data..other sections, which Linux
 * has defined. Same for text and bss.
 *
 * With LTO_CLANG, the linker also splits sections by default, so we need
 * these macros to combine the sections during the final link.
 *
 * RODATA_MAIN is not used because existing code already defines .rodata.x
 * sections to be brought in with rodata.
 */
#if defined(CONFIG_LD_DEAD_CODE_DATA_ELIMINATION)
#define TEXT_MAIN   .text .text.[0-9a-zA-Z_]*
#define DATA_MAIN   .data .data.[0-9a-zA-Z_]* .data..L* .data..compoundliteral* .data.$__unnamed_* .data.$L*
#define SDATA_MAIN  .sdata .sdata.[0-9a-zA-Z_]*
#define RODATA_MAIN .rodata .rodata.[0-9a-zA-Z_]* .rodata..L*
#define BSS_MAIN    .bss .bss.[0-9a-zA-Z_]* .bss..compoundliteral*
#define SBSS_MAIN   .sbss .sbss.[0-9a-zA-Z_]*
#else
#define TEXT_MAIN   .text
#define DATA_MAIN   .data
#define SDATA_MAIN  .sdata
#define RODATA_MAIN .rodata
#define BSS_MAIN    .bss
#define SBSS_MAIN   .sbss
#endif

/* The actual configuration determine if the init/exit sections
 * are handled as text/data or they can be discarded (which
 * often happens at runtime)
 */
#ifdef CONFIG_HOTPLUG_CPU
#define CPU_KEEP(sec)    *(.cpu##sec)
#define CPU_DISCARD(sec)
#else
#define CPU_KEEP(sec)
#define CPU_DISCARD(sec) *(.cpu##sec)
#endif

#if defined(CONFIG_MEMORY_HOTPLUG)
#define MEM_KEEP(sec)    *(.mem##sec)
#define MEM_DISCARD(sec)
#else
#define MEM_KEEP(sec)
#define MEM_DISCARD(sec) *(.mem##sec)
#endif

/*
 * RO data: RO data
 */
#define RODATA_RODATA(align)                                \
    _ld_rodata_rodata_start = .;                            \
    *(RODATA_MAIN)                                          \
    _ld_rodata_rodata_end = .;

#define KSYMTAB(align)                                      \
    _ld_ksymtab_start = .;                                  \
    KEEP(*(SORT(.ksymtab+*)))                               \
    _ld_ksymtab_end = .;

#define KSYMTAB_GPL(align)                                  \
    _ld_ksymtab_gpl_start = .;                              \
    KEEP(*(SORT(.ksymtab_gpl+*)))                           \
    _ld_ksymtab_gpl_end = .;

#define NOTES                                               \
    .notes :                                                \
    AT(ADDR(.notes) - LOAD_OFFSET) {                        \
        __start_notes = .;                                  \
        KEEP(*(.note.*))                                    \
        __stop_notes = .;                                   \
    } NOTES_HEADERS                                         \
    NOTES_HEADERS_RESTORE

/*
 * Init text: init text
 */
#define INIT_TEXT                                           \
    *(.init.text .init.text.*)                              \
    *(.text.startup)                                        \
    MEM_DISCARD(init.text*)

/*
 * Init data: DT table
 */
#define INIT_DT_TABLES                                      \
    _ld_init_dtb_start = .;                                 \
    KEEP(*(.init.dtb));                                     \
    _ld_init_dtb_end = .;

/*
 * Init data: Boot param table
 */
#define INIT_ARGS(align)                                    \
    . = ALIGN(align);                                       \
    _ld_boot_param_start = .;                               \
    KEEP(*(.init.bootargs))                                 \
    _ld_boot_param_end = .;

/*
 * Init data: Initcall
 */
#define INIT_CALLS_LEVEL(level)                             \
    _ld_initcall##level##_start = .;                        \
    KEEP(*(.init.initcall_##level##))                       \
    KEEP(*(.init.initcall_##level##s))                      \

#define INIT_CALLS                                          \
    _ld_initcall_start = .;                                 \
    INIT_CALLS_LEVEL(0)                                     \
    INIT_CALLS_LEVEL(1)                                     \
    INIT_CALLS_LEVEL(2)                                     \
    INIT_CALLS_LEVEL(3)                                     \
    INIT_CALLS_LEVEL(4)                                     \
    INIT_CALLS_LEVEL(5)                                     \
    INIT_CALLS_LEVEL(6)                                     \
    INIT_CALLS_LEVEL(7)                                     \
    INIT_CALLS_LEVEL(8)                                     \
    _ld_initcall_end = .;

#define CONSOLE_INITCALL                                    \
    _ld_pre_console_initcall_start = .;                     \
    KEEP(*(.init.pre_console_initcall))                     \
    _ld_console_initcall_start = .;                         \
    KEEP(*(.init.console_initcall))                         \
    _ld_console_initcall_end = .;

#define SCHEDULER_INITCALL                                  \
    _ld_scheduler_initcall_start = .;                       \
    KEEP(*(.init.scheduler_initcall))                       \
    _ld_scheduler_initcall_end = .;

#define CLK_INITCALL                                        \
    _ld_clk_initcall_start = .;                             \
    KEEP(*(.init.clk_initcall))                             \
    _ld_clk_initcall_end = .;

#define IRQCHIP_INITCALL                                    \
    _ld_irqchip_initcall_start = .;                         \
    KEEP(*(.init.irqchip_initcall))                         \
    _ld_irqchip_initcall_end = .;

#define CLKSRC_INITCALL                                     \
    _ld_clocksource_initcall_start = .;                     \
    KEEP(*(.init.clocksource_initcall))                     \
    _ld_clocksource_initcall_end = .;

#define KSHELL_INITCALL                                     \
    _ld_kshell_initcall_start = .;                          \
    KEEP(*(.init.kshell_initcall))                          \
    _ld_kshell_initcall_end = .;

/*
 * bss (Block Started by Symbol) - uninitialized data
 * zeroed during startup
 */
#define SBSS(align)                                         \
    . = ALIGN(align);                                       \
    .sbss :                                                 \
    AT(ADDR(.sbss) - LOAD_OFFSET) {                         \
        *(.dynsbss)                                         \
        *(SBSS_MAIN)                                        \
        *(.scommon)                                         \
    }

#define BSS(align)                                          \
    . = ALIGN(align);                                       \
    .bss :                                                  \
    AT(ADDR(.bss) - LOAD_OFFSET) {                          \
        . = ALIGN(PAGE_SIZE);                               \
        *(.bss..page_aligned)                               \
        . = ALIGN(PAGE_SIZE);                               \
        *(.dynbss)                                          \
        *(BSS_MAIN)                                         \
        *(COMMON)                                           \
    }

#define INIT_TASK_STACK(align)                              \
    . = ALIGN(align);                                       \
    .task_stack :                                           \
    AT(ADDR(.task_stack) - LOAD_OFFSET) {                   \
    _ld_init_task_stack_start = .;                          \
    init_thread_union = .;                                  \
    . = _ld_init_task_stack_start + THREAD_SIZE;            \
    _ld_init_task_stack_end = .;                            \
    }

#define COMMON_DISCARDS                                     \
    *(.eh_frame)                                            \
    *(.discard)                                             \
    *(.discard.*)                                           \
    *(.modinfo)                                             \
    *(.gnu.version*)                                        \

#define DISCARDS                                            \
    /DISCARD/ : {                                           \
    COMMON_DISCARDS                                         \
    }

/*
 * Definition of the high level *_SECTION macros
 * They will fit only a subset of the architectures
 */

#define STARTUP_SECTION                                     \
    .startup :                                              \
    AT(ADDR(.startup) - LOAD_OFFSET) {                      \
        PROVIDE(_ld_startup_start = .);                     \
        KEEP(*(.startup))                                   \
        PROVIDE(_ld_startup_end = .);                       \
    }                                                       \
PROVIDE(_ld_head_size = _ld_startup_end - _ld_startup_start);

#define TEXT_SECTION(pagealign)                             \
    . = ALIGN(pagealign);                                   \
    .text :                                                 \
    AT(ADDR(.text) - LOAD_OFFSET) {                         \
        _ld_text_start = .;                                 \
        *(TEXT_MAIN)                                        \
        _ld_text_end = .;                                   \
    }

#define RODATA_SECTION(pagealign)                           \
    . = ALIGN(pagealign);                                   \
    .rodata :                                               \
    AT(ADDR(.rodata) - LOAD_OFFSET) {                       \
        RODATA_RODATA(pagealign)                            \
        KSYMTAB(pagealign)                                  \
        KSYMTAB_GPL(pagealign)                              \
    }                                                       \
    NOTES

#define RWDATA_SECTION(cacheline, pagealign)                \
    . = ALIGN(pagealign);                                   \
    .data :                                                 \
    AT(ADDR(.data) - LOAD_OFFSET) {                         \
        _ld_data_start = .;                                 \
        *(DATA_MAIN)                                        \
        _ld_data_end = .;                                   \
    }

#define PERCPU_SECTION(cacheline, pagealign)                \
    . = ALIGN(pagealign);                                   \
    .percpu :                                               \
    AT(ADDR(.data) - LOAD_OFFSET) {                         \
    }


#define INIT_TEXT_SECTION(pagealign)                        \
    . = ALIGN(pagealign);                                   \
    .init.text :                                            \
    AT(ADDR(.init.text) - LOAD_OFFSET) {                    \
        _ld_inittext_start = .;                             \
        INIT_TEXT                                           \
        _ld_inittext_end = .;                               \
    }

#define INIT_DATA_SECTION(align)                            \
    .init.data :                                            \
    AT(ADDR(.init.data) - LOAD_OFFSET) {                    \
        _ld_data_section_start = .;                         \
        CONSOLE_INITCALL                                    \
        SCHEDULER_INITCALL                                  \
        CLK_INITCALL                                        \
        IRQCHIP_INITCALL                                    \
        CLKSRC_INITCALL                                     \
        KSHELL_INITCALL                                     \
        INIT_CALLS                                          \
        INIT_ARGS(align)                                    \
        INIT_DT_TABLES                                      \
        _ld_data_section_end = .;                           \
    }

#define BSS_SECTION(sbss_align, bss_align, stop_align)      \
    . = ALIGN(sbss_align);                                  \
    _ld_bss_start = .;                                      \
    SBSS(sbss_align)                                        \
    BSS(bss_align)                                          \
    INIT_TASK_STACK(bss_align)                              \
    . = ALIGN(stop_align);                                  \
    _ld_bss_end = .;

#define DWARF_DEBUG                                         \
    /* DWARF 1 */                                           \
    .debug          0 : { *(.debug) }                       \
    .line           0 : { *(.line) }                        \
    /* GNU DWARF 1 extensions */                            \
    .debug_srcinfo  0 : { *(.debug_srcinfo) }               \
    .debug_sfnames  0 : { *(.debug_sfnames) }               \
    /* DWARF 1.1 and DWARF 2 */                             \
    .debug_aranges  0 : { *(.debug_aranges) }               \
    .debug_pubnames 0 : { *(.debug_pubnames) }              \
    /* DWARF 2 */                                           \
    .debug_info     0 : { *(.debug_info                     \
            .gnu.linkonce.wi.*) }                           \
    .debug_abbrev   0 : { *(.debug_abbrev) }                \
    .debug_line     0 : { *(.debug_line) }                  \
    .debug_frame    0 : { *(.debug_frame) }                 \
    .debug_str      0 : { *(.debug_str) }                   \
    .debug_loc      0 : { *(.debug_loc) }                   \
    .debug_macinfo  0 : { *(.debug_macinfo) }               \
    .debug_pubtypes 0 : { *(.debug_pubtypes) }              \
    /* DWARF 3 */                                           \
    .debug_ranges    0 : { *(.debug_ranges) }               \
    /* SGI/MIPS DWARF 2 extensions */                       \
    .debug_weaknames 0 : { *(.debug_weaknames) }            \
    .debug_funcnames 0 : { *(.debug_funcnames) }            \
    .debug_typenames 0 : { *(.debug_typenames) }            \
    .debug_varnames  0 : { *(.debug_varnames) }             \
    /* GNU DWARF 2 extensions */                            \
    .debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }      \
    .debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }      \
    /* DWARF 4 */                                           \
    .debug_types    0 : { *(.debug_types) }                 \
    /* DWARF 5 */                                           \
    .debug_addr    0 : { *(.debug_addr) }                   \
    .debug_line_str    0 : { *(.debug_line_str) }           \
    .debug_loclists    0 : { *(.debug_loclists) }           \
    .debug_macro    0 : { *(.debug_macro) }                 \
    .debug_names    0 : { *(.debug_names) }                 \
    .debug_rnglists    0 : { *(.debug_rnglists) }           \
    .debug_str_offsets    0 : { *(.debug_str_offsets) }

#endif  /* _ASM_GENERIC_KERNEL_LDS_H_ */
