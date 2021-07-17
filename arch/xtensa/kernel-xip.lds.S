OUTPUT_FORMAT("elf32-xtensa-le")
OUTPUT_ARCH(xtensa)

#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include <asm/core.h>
#include <asm/vectors.h>

/* Entry Point */
ENTRY(boot_head)

#define xip_align   4
#define ram_align   4

/*
 * Macro to define a section for a vector. When MERGED_VECTORS is 0
 * code for every vector is located with other init data. At startup
 * time head.S copies code for every vector to its final position according
 * to description recorded in the corresponding RELOCATE_ENTRY.
 */

#define SECTION_VECTOR4(sym, section, addr, prevsec)			    \
  section addr : AT(((LOADADDR(prevsec) + SIZEOF(prevsec)) + 3) & ~ 3)      \
  {									    \
    . = ALIGN(4);							    \
    sym ## _start = ABSOLUTE(.);		 			    \
    *(section)								    \
    sym ## _end = ABSOLUTE(.);						    \
  }

#define SECTION_VECTOR2(section, addr)					    \
  . = addr;								    \
  *(section)


/* Define output sections */
SECTIONS
{
    . = CONFIG_XIP_BASE;
    
    STARTUP_SECTION 
    
    
    .vectors :
    {
        _vecbase = .;
#if XCHAL_HAVE_WINDOWED
        SECTION_VECTOR2 (.WindowVectors.text, WINDOW_VECTORS_VADDR)
#endif
#if XCHAL_EXCM_LEVEL >= 2
        SECTION_VECTOR2 (.Level2InterruptVector.text, INTLEVEL2_VECTOR_VADDR)
#endif
#if XCHAL_EXCM_LEVEL >= 3
        SECTION_VECTOR2 (.Level3InterruptVector.text, INTLEVEL3_VECTOR_VADDR)
#endif
#if XCHAL_EXCM_LEVEL >= 4
        SECTION_VECTOR2 (.Level4InterruptVector.text, INTLEVEL4_VECTOR_VADDR)
#endif
#if XCHAL_EXCM_LEVEL >= 5
        SECTION_VECTOR2 (.Level5InterruptVector.text, INTLEVEL5_VECTOR_VADDR)
#endif
#if XCHAL_EXCM_LEVEL >= 6
        SECTION_VECTOR2 (.Level6InterruptVector.text, INTLEVEL6_VECTOR_VADDR)
#endif
        SECTION_VECTOR2 (.DebugInterruptVector.text, DEBUG_VECTOR_VADDR)
        SECTION_VECTOR2 (.NMIInterruptVector.text, NMI_VECTOR_VADDR)
        SECTION_VECTOR2 (.KernelExceptionVector.text, KERNEL_VECTOR_VADDR)
        SECTION_VECTOR2 (.UserExceptionVector.text, USER_VECTOR_VADDR)
        SECTION_VECTOR2 (.DoubleExceptionVector.text, DOUBLEEXC_VECTOR_VADDR)
    }
    
    
    TEXT_SECTION(xip_align) 
    
    INIT_TEXT_SECTION(xip_align)
    
/*
 * From this point, stuff is considered writable and will be copied to RAM
 */
    _ld_load_offset = ALIGN(4);
    . = PAGE_OFFSET;        /* location in memory */
#undef LOAD_OFFSET
#define LOAD_OFFSET (PAGE_OFFSET - _ld_load_offset)
    
    RODATA_SECTION(ram_align) 
    
    RWDATA_SECTION(ram_align, ram_align, ram_align)
    
    INIT_DATA_SECTION(ram_align)
    
    _ld_load_end = .;
    
    PROVIDE(_ld_kernel_size = . - CONFIG_XIP_BASE - LOAD_OFFSET);
    
    BSS_SECTION(ram_align, ram_align, ram_align)

#undef LOAD_OFFSET

    DWARF_DEBUG  
    
    /* Sections to be discarded */
    DISCARDS
}

