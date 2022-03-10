/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include <asm/cache.h>

#if defined(CONFIG_ARCH_X86_32)
OUTPUT_ARCH(i386)
#elif defined(CONFIG_ARCH_X86_64)
OUTPUT_ARCH(i386:x86-64)
#else
# error "unknown instruction set"
#endif

ENTRY(startup_head)

SECTIONS {
    . = pa_to_va(NORMAL_OFFSET);
    _ld_image_start = .;

    STARTUP_SECTION
    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)
    RWDATA_SECTION(CACHE_LINE_SIZE, PAGE_SIZE)

#ifdef CONFIG_SMP
    PERCPU_SECTION(CACHE_LINE_SIZE, PAGE_SIZE)
#endif

    INIT_TEXT_SECTION(PAGE_SIZE)
    INIT_DATA_SECTION(PAGE_SIZE)

    EXIT_TEXT_SECTION(PAGE_SIZE)
    EXIT_DATA_SECTION(PAGE_SIZE)

    BSS_SECTION(PAGE_SIZE, PAGE_SIZE, PAGE_SIZE)

    _ld_image_end = .;

    DWARF_DEBUG
    DISCARDS

    .got : {
        *(.got) *(.igot.*)
    }

    .plt : {
        *(.plt) *(.plt.*) *(.iplt)
    }

    .rel.dyn : {
        *(.rel.*) *(.rel_*)
    }

    .rela.dyn : {
        *(.rela.*) *(.rela_*)
    }
}

ASSERT(SIZEOF(.got) == 0, "Unexpected GOT entries detected!")
ASSERT(SIZEOF(.plt) == 0, "Unexpected run-time procedure linkages detected!")
ASSERT(SIZEOF(.rel.dyn) == 0, "Unexpected run-time relocations (.rel) detected!")
ASSERT(SIZEOF(.rela.dyn) == 0, "Unexpected run-time relocations (.rela) detected!")
PROVIDE(_ld_image_size = _ld_image_end - _ld_image_start);
