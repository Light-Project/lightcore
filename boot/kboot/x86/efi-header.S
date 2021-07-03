#include <pe.h>
#include <size.h>

.macro  EFI_HEADER

pe_header:
        .long    PE_MAGIC

coff_header:
#ifdef CONFIG_X86_32
        .set    image_file_add_flags, IMAGE_FILE_32BIT_MACHINE
        .set    pe_opt_magic, PE_OPT_MAGIC_PE32
        .word   IMAGE_FILE_MACHINE_I386         # Machine
#else
        .set    image_file_add_flags, 0
        .set    pe_opt_magic, PE_OPT_MAGIC_PE32PLUS
        .word   IMAGE_FILE_MACHINE_AMD64        # Machine
#endif
        .short  section_count                   # NumberOfSections
        .long   0                               # TimeDateStamp
        .long   0                               # PointerToSymbolTable
        .long   1                               # NumberOfSymbols
        .short  section_table - optional_header # SizeOfOptionalHeader
        .word   IMAGE_FILE_EXECUTABLE_IMAGE | \
                image_file_add_flags        | \
                IMAGE_FILE_DEBUG_STRIPPED   | \
                IMAGE_FILE_LINE_NUMS_STRIPPED   # Characteristics

optional_header:
        .word    pe_opt_magic
        .byte    0x02                           # MajorLinkerVersion
        .byte    0x14                           # MinorLinkerVersion
        .long    0                              # SizeOfCode
        .long    0                              # SizeOfInitializedData
        .long    0                              # SizeOfUninitializedData
        .long    0x0000                         # AddressOfEntryPoint
        .long    0x0200                         # BaseOfCode
#ifdef CONFIG_X86_32
        .long    0                              # data
#endif

extra_header_fields:
        # PE specification requires ImageBase to be 64k aligned
        .set    image_base, (0 + 0xffff) & ~0xffff
    
#ifdef CONFIG_X86_32
        .long    image_base                     # ImageBase
#else
        .quad    image_base                     # ImageBase
#endif

        .long    0x20                           # SectionAlignment
        .long    0x20                           # FileAlignment
        .word    0                              # MajorOperatingSystemVersion
        .word    0                              # MinorOperatingSystemVersion
        .word    LINUX_EFISTUB_MAJOR_VERSION    # MajorImageVersion
        .word    LINUX_EFISTUB_MINOR_VERSION    # MinorImageVersion
        .word    0                              # MajorSubsystemVersion
        .word    0                              # MinorSubsystemVersion
        .long    0                              # Win32VersionValue

        #
        # The size of the bzImage is written in tools/build.c
        #
        .long    0                              # SizeOfImage

        .long    0x200                          # SizeOfHeaders
        .long    0                              # CheckSum
        .word    IMAGE_SUBSYSTEM_EFI_APPLICATION# Subsystem (EFI application)
        .word    0                              # DllCharacteristics

#ifdef CONFIG_X86_32
        .long    0                              # SizeOfStackReserve
        .long    0                              # SizeOfStackCommit
        .long    0                              # SizeOfHeapReserve
        .long    0                              # SizeOfHeapCommit
#else
        .quad    0                              # SizeOfStackReserve
        .quad    0                              # SizeOfStackCommit
        .quad    0                              # SizeOfHeapReserve
        .quad    0                              # SizeOfHeapCommit
#endif

        .long    0                              # LoaderFlags
        .long    (section_table - .) / 8        # NumberOfRvaAndSizes
        .quad    0                              # ExportTable
        .quad    0                              # ImportTable
        .quad    0                              # ResourceTable
        .quad    0                              # ExceptionTable
        .quad    0                              # CertificationTable
        .quad    0                              # BaseRelocationTable

section_table:
        .ascii  ".text\0\0\0"
        .long   __pecoff_code_size              # VirtualSize
        .long   __efi_start                     # VirtualAddress
        .long   __pecoff_code_size              # SizeOfRawData
        .long   __efi_start                     # PointerToRawData
        .long   0                               # PointerToRelocations
        .long   0                               # PointerToLineNumbers
        .short  0                               # NumberOfRelocations
        .short  0                               # NumberOfLineNumbers
        .long   IMAGE_SCN_CNT_CODE | \
                IMAGE_SCN_MEM_READ | \
                IMAGE_SCN_MEM_EXECUTE           # Characteristics

    .set    section_count, (. - section_table) / 40
.endm
