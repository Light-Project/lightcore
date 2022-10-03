/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_KERNEL_LDS_H_
#define _ASM_GENERIC_KERNEL_LDS_H_

#ifndef LOAD_OFFSET
# define LOAD_OFFSET 0
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
# define NOTES_HEADERS          :text :note
# define NOTES_HEADERS_RESTORE  __restore_ph : { *(.__restore_ph) } :text
#else
# define NOTES_HEADERS
# define NOTES_HEADERS_RESTORE
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
# define TEXT_MAIN      .text .text.[0-9a-zA-Z_]*
# define DATA_MAIN      .data .data.[0-9a-zA-Z_]* .data..L* .data..compoundliteral* .data.$__unnamed_* .data.$L*
# define SDATA_MAIN     .sdata .sdata.[0-9a-zA-Z_]*
# define RODATA_MAIN    .rodata .rodata.[0-9a-zA-Z_]* .rodata..L*
# define BSS_MAIN       .bss .bss.[0-9a-zA-Z_]* .bss..compoundliteral*
# define SBSS_MAIN      .sbss .sbss.[0-9a-zA-Z_]*
#else
# define TEXT_MAIN      .text
# define DATA_MAIN      .data
# define SDATA_MAIN     .sdata
# define RODATA_MAIN    .rodata .rodata.*
# define BSS_MAIN       .bss
# define SBSS_MAIN      .sbss
#endif

/*
 * The actual configuration determine if the init/exit sections
 * are handled as text/data or they can be discarded (which
 * often happens at runtime)
 */
#ifdef CONFIG_HOTPLUG_CPU
# define CPU_KEEP(sec)      *(.cpu##sec)
# define CPU_DISCARD(sec)
#else
# define CPU_KEEP(sec)
# define CPU_DISCARD(sec)   *(.cpu##sec)
#endif

#if defined(CONFIG_MEMORY_HOTPLUG)
# define MEM_KEEP(sec)      *(.mem##sec)
# define MEM_DISCARD(sec)
#else
# define MEM_KEEP(sec)
# define MEM_DISCARD(sec)   *(.mem##sec)
#endif

/*
 * Text Segment
 */

#define TEXT_TEXT(align)                                    \
    . = ALIGN(align);                                       \
    *(.text.hot .text.hot.*)                                \
    *(TEXT_MAIN)                                            \
    *(.text.unlikely .text.unlikely.*)                      \
    *(.text.unknown .text.unknown.*)                        \
    *(.text.asan.* .text.tsan.*)                            \

#define TEXT_SCHED(align)                                   \
    . = ALIGN(align);                                       \
    _ld_text_sched_start = .;                               \
    *(.text.sched)                                          \
    _ld_text_sched_end = .;

#define TEXT_CPUIDLE(align)                                 \
    . = ALIGN(align);                                       \
    _ld_text_cpuidle_start = .;                             \
    *(.text.cpuidle)                                        \
    _ld_text_cpuidle_end = .;

#define TEXT_ENTRY(align)                                   \
    . = ALIGN(align);                                       \
    _ld_text_entry_start = .;                               \
    *(.text.entry)                                          \
    _ld_text_entry_end = .;

#define TEXT_IRQENTRY(align)                                \
    . = ALIGN(align);                                       \
    _ld_text_irqentry_start = .;                            \
    *(.text.irqentry)                                       \
    _ld_text_irqentry_end = .;

#define TEXT_SOFTIRQENTRY(align)                            \
    . = ALIGN(align);                                       \
    _ld_text_softirqentry_start = .;                        \
    *(.text.softirqentry)                                   \
    _ld_text_softirqentry_end = .;

/*
 * RO data Segment
 */

#define RODATA_RODATA(align)                                \
    . = ALIGN(align);                                       \
    _ld_rodata_rodata_start = .;                            \
    *(RODATA_MAIN)                                          \
    _ld_rodata_rodata_end = .;

#define KSYMTAB(align)                                      \
    . = ALIGN(align);                                       \
    _ld_ksymtab_start = .;                                  \
    KEEP(*(SORT(.ksymtab*)))                                \
    _ld_ksymtab_end = .;

#define KSYMTAB_GPL(align)                                  \
    . = ALIGN(align);                                       \
    _ld_ksymtab_gpl_start = .;                              \
    KEEP(*(SORT(.ksymtab_gpl*)))                            \
    _ld_ksymtab_gpl_end = .;

#define EXCEPTION_TABLE(align)                              \
    . = ALIGN(align);                                       \
    _ld_extable_start = .;                                  \
    KEEP(*(.extable))                                       \
    _ld_extable_end = .;

#define ROMDISK(align)                                      \
    . = ALIGN(align);                                       \
    _ld_romdisk_start = .;                                  \
    KEEP(*(.romdisk))                                       \
    _ld_romdisk_end = .;

#define NOTES                                               \
    .notes :                                                \
    AT(ADDR(.notes) - LOAD_OFFSET) {                        \
        _ld_notes_start = .;                                \
        KEEP(*(.note.*))                                    \
        _ld_notes_end = .;                                  \
    } NOTES_HEADERS                                         \
    NOTES_HEADERS_RESTORE

/*
 * RW data Segment
 */

#define READ_MOSTLY(align)                                  \
    . = ALIGN(align);                                       \
    _ld_data_read_mostly_start = .;                         \
    *(.data.readmostly*)                                    \
    . = ALIGN(align);                                       \
    _ld_data_read_mostly_end = .;

#define BUG_TABLE(align)                                    \
    . = ALIGN(align);                                       \
    _ld_data_bug_table_start = .;                           \
    *(.data.bug_table*)                                     \
    . = ALIGN(align);                                       \
    _ld_data_bug_table_end = .;

/*
 * Init Text Segment
 */

#define INIT_TEXT                                           \
    *(.init.text)                                           \
    *(.init.text.*)                                         \
    *(.text.startup)                                        \
    MEM_DISCARD(init.text*)

/*
 * Init Data Segment
 */

#define INIT_DATA                                           \
    *(.init.rodata)                                         \
    *(.init.rodata.*)                                       \
    *(.init.data)                                           \
    *(.init.data.*)                                         \
    MEM_DISCARD(init.rodata*)                               \
    MEM_DISCARD(init.data*)

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
    _ld_pconsole_initcall_start = .;                        \
    KEEP(*(.init.pconsole_initcall))                        \
    _ld_pconsole_initcall_end = .;                          \
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

#define CLKEVT_INITCALL                                     \
    _ld_clockevent_initcall_start = .;                      \
    KEEP(*(.init.clockevent_initcall))                      \
    _ld_clockevent_initcall_end = .;

#define CLKSRC_INITCALL                                     \
    _ld_clocksource_initcall_start = .;                     \
    KEEP(*(.init.clocksource_initcall))                     \
    _ld_clocksource_initcall_end = .;

#define KSHELL_INITCALL                                     \
    _ld_kshell_initcall_start = .;                          \
    KEEP(*(.init.kshell_initcall))                          \
    _ld_kshell_initcall_end = .;

#define INIT_DT_TABLES                                      \
    _ld_init_dtb_start = .;                                 \
    KEEP(*(.init.dtb));                                     \
    _ld_init_dtb_end = .;

#define INIT_ARGS(align)                                    \
    . = ALIGN(align);                                       \
    _ld_boot_param_start = .;                               \
    KEEP(*(.init.bootargs))                                 \
    _ld_boot_param_end = .;

#define INIT_CTORS(align)                                   \
    . = ALIGN(align);                                       \
    _ld_ctors_start = .;                                    \
    KEEP(*(.ctors))                                         \
    KEEP(*(SORT(.ctors.*)))                                 \
    KEEP(*(.init_array))                                    \
    KEEP(*(SORT(.init_array.*)))                            \
    _ld_ctors_end = .;

/*
 * Exit Text Segment
 */

#define EXIT_TEXT                                           \
    *(.exit.text .exit.text.*)                              \
    *(.text.exit .text.exit.*)                              \
    MEM_DISCARD(exit.text*)

/*
 * Exit Data Segment
 */

#define EXIT_DATA                                           \
    *(.exit.rodata .exit.rodata.*)                          \
    *(.exit.data .exit.data.*)                              \
    *(.fini_array .fini_array.*)                            \
    *(.dtors .dtors.*)                                      \
    MEM_DISCARD(exit.rodata*)                               \
    MEM_DISCARD(exit.data*)

/*
 * bss (Block Started by Symbol) - uninitialized data
 * zeroed during startup
 */
#define SBSS(align)                                         \
    . = ALIGN(align);                                       \
    *(.dynsbss)                                             \
    *(SBSS_MAIN)                                            \
    *(.scommon)

#define BSS(align, pagealign)                               \
    . = ALIGN(align);                                       \
    *(BSS_MAIN)                                             \
    *(.dynbss)                                              \
    *(COMMON)                                               \
    . = ALIGN(pagealign);                                   \
    *(.bss..page_aligned)                                   \
    . = ALIGN(pagealign);

#define INIT_TASK_STACK(align)                              \
    . = ALIGN(align);                                       \
    _ld_init_task_stack_start = .;                          \
    init_thread_union = .;                                  \
    init_stack = .;                                         \
    . = _ld_init_task_stack_start + THREAD_SIZE;            \
    _ld_init_task_stack_end = .;                            \

#define COMMON_DISCARDS                                     \
    *(.eh_frame)                                            \
    *(.discard)                                             \
    *(.discard.*)                                           \
    *(.modinfo)                                             \
    *(.gnu.version*)                                        \

#define DISCARDS                                            \
    /DISCARD/ : {                                           \
        COMMON_DISCARDS                                     \
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
        KEEP(*(.startup.*))                                 \
        PROVIDE(_ld_startup_end = .);                       \
    }                                                       \
PROVIDE(_ld_head_size = _ld_startup_end - _ld_startup_start);

#define TEXT_SECTION(cacheline, pagealign)                  \
    . = ALIGN(pagealign);                                   \
    .text :                                                 \
    AT(ADDR(.text) - LOAD_OFFSET) {                         \
        _ld_text_start = .;                                 \
        TEXT_TEXT(cacheline)                                \
        TEXT_SCHED(cacheline)                               \
        TEXT_CPUIDLE(cacheline)                             \
        TEXT_ENTRY(cacheline)                               \
        TEXT_IRQENTRY(cacheline)                            \
        TEXT_SOFTIRQENTRY(cacheline)                        \
        _ld_text_end = .;                                   \
    }

#define RODATA_SECTION(cacheline, pagealign)                \
    . = ALIGN(pagealign);                                   \
    .rodata :                                               \
    AT(ADDR(.rodata) - LOAD_OFFSET) {                       \
        _ld_rodata_start = .;                               \
        RODATA_RODATA(pagealign)                            \
        KSYMTAB(pagealign)                                  \
        KSYMTAB_GPL(pagealign)                              \
        EXCEPTION_TABLE(pagealign)                          \
        ROMDISK(pagealign)                                  \
        _ld_rodata_end = .;                                 \
    }                                                       \
    NOTES

#define RWDATA_SECTION(cacheline, pagealign)                \
    . = ALIGN(pagealign);                                   \
    .data :                                                 \
    AT(ADDR(.data) - LOAD_OFFSET) {                         \
        _ld_data_start = .;                                 \
        *(DATA_MAIN)                                        \
        READ_MOSTLY(cacheline)                              \
        BUG_TABLE(cacheline)                                \
        _ld_data_end = .;                                   \
    }

#define PERCPU_SECTION(cacheline, pagealign)                \
    . = ALIGN(pagealign);                                   \
    .percpu :                                               \
    AT(ADDR(.percpu) - LOAD_OFFSET) {                       \
    }

#define INIT_TEXT_SECTION(cacheline, pagealign)             \
    . = ALIGN(pagealign);                                   \
    .init.text :                                            \
    AT(ADDR(.init.text) - LOAD_OFFSET) {                    \
        _ld_inittext_start = .;                             \
        INIT_TEXT                                           \
        _ld_inittext_end = .;                               \
    }

#define EXIT_TEXT_SECTION(cacheline, pagealign)             \
    . = ALIGN(pagealign);                                   \
    .exit.text :                                            \
    AT(ADDR(.exit.text) - LOAD_OFFSET) {                    \
        _ld_exittext_start = .;                             \
        EXIT_TEXT                                           \
        _ld_exittext_end = .;                               \
    }

#define INIT_DATA_SECTION(cacheline, pagealign)             \
    . = ALIGN(pagealign);                                   \
    .init.data :                                            \
    AT(ADDR(.init.data) - LOAD_OFFSET) {                    \
        _ld_initdata_start = .;                             \
        INIT_DATA                                           \
        CONSOLE_INITCALL                                    \
        SCHEDULER_INITCALL                                  \
        CLK_INITCALL                                        \
        IRQCHIP_INITCALL                                    \
        CLKEVT_INITCALL                                     \
        CLKSRC_INITCALL                                     \
        KSHELL_INITCALL                                     \
        INIT_CALLS                                          \
        INIT_ARGS(cacheline)                                \
        INIT_CTORS(cacheline)                               \
        INIT_DT_TABLES                                      \
        _ld_initdata_end = .;                               \
    }


#define EXIT_DATA_SECTION(cacheline, pagealign)             \
    . = ALIGN(pagealign);                                   \
    .exit.data :                                            \
    AT(ADDR(.exit.data) - LOAD_OFFSET) {                    \
        _ld_exitdata_start = .;                             \
        EXIT_DATA                                           \
        _ld_exitdata_end = .;                               \
    }

#define BSS_SECTION(sbss_align, bss_align, pagealign)       \
    . = ALIGN(pagealign);                                   \
    .bss :                                                  \
    AT(ADDR(.bss) - LOAD_OFFSET) {                          \
        _ld_bss_start = .;                                  \
        SBSS(sbss_align)                                    \
        BSS(bss_align, pagealign)                           \
        INIT_TASK_STACK(bss_align)                          \
        . = ALIGN(pagealign);                               \
        _ld_bss_end = .;                                    \
    }

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

/* Stabs debugging sections. */
#define STABS_DEBUG                                         \
    .stab 0 : { *(.stab) }                                  \
    .stabstr 0 : { *(.stabstr) }                            \
    .stab.excl 0 : { *(.stab.excl) }                        \
    .stab.exclstr 0 : { *(.stab.exclstr) }                  \
    .stab.index 0 : { *(.stab.index) }                      \
    .stab.indexstr 0 : { *(.stab.indexstr) }

/* Required sections not related to debugging. */
#define ELF_DETAILS                                         \
    .comment 0 : { *(.comment) }                            \
    .symtab 0 : { *(.symtab) }                              \
    .strtab 0 : { *(.strtab) }                              \
    .shstrtab 0 : { *(.shstrtab) }

#endif  /* _ASM_GENERIC_KERNEL_LDS_H_ */
