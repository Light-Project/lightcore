/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ELF_H_
#define _LIGHTCORE_ELF_H_

#include <lightcore/types.h>

typedef uint32_t    Elf32_Addr;
typedef uint16_t    Elf32_Half;
typedef uint32_t    Elf32_Off;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf32_Word;

typedef uint64_t    Elf64_Addr;
typedef uint16_t    Elf64_Half;
typedef int16_t     Elf64_SHalf;
typedef uint64_t    Elf64_Off;
typedef int32_t     Elf64_Sword;
typedef uint32_t    Elf64_Word;
typedef uint64_t    Elf64_Xword;
typedef int64_t     Elf64_Sxword;

enum elf_machine {
    ELF_MACHINE_NONE            = 0,
    ELF_MACHINE_M32             = 1,
    ELF_MACHINE_SPARC           = 2,
    ELF_MACHINE_386             = 3,
    ELF_MACHINE_68K             = 4,
    ELF_MACHINE_88K             = 5,
    ELF_MACHINE_486             = 6,
    ELF_MACHINE_860             = 7,
    ELF_MACHINE_MIPS            = 8,
    ELF_MACHINE_MIPS_RS3_LE     = 10,
    ELF_MACHINE_MIPS_RS4_BE     = 10,

    ELF_MACHINE_PARISC          = 15,
    ELF_MACHINE_SPARC32PLUS     = 18,
    ELF_MACHINE_PPC             = 20,
    ELF_MACHINE_PPC64           = 21,
    ELF_MACHINE_SPU             = 23,
    ELF_MACHINE_ARM             = 40,
    ELF_MACHINE_SH              = 42,
    ELF_MACHINE_SPARCV9         = 43,
    ELF_MACHINE_H8_300          = 46,
    ELF_MACHINE_IA_64           = 50,
    ELF_MACHINE_X86_64          = 62,
    ELF_MACHINE_S390            = 22,
    ELF_MACHINE_CRIS            = 76,
    ELF_MACHINE_M32R            = 88,
    ELF_MACHINE_MN10300         = 89,
    ELF_MACHINE_OPENRISC        = 92,
    ELF_MACHINE_ARCOMPACT       = 93,
    ELF_MACHINE_XTENSA          = 94,
    ELF_MACHINE_BLACKFIN        = 106,
    ELF_MACHINE_UNICORE         = 110,
    ELF_MACHINE_ALTERA_NIOS2    = 113,
    ELF_MACHINE_TI_C6000        = 140,
    ELF_MACHINE_HEXAGON         = 164,
    ELF_MACHINE_NDS32           = 167,

    ELF_MACHINE_AARCH64         = 183,
    ELF_MACHINE_TILEPRO         = 188,
    ELF_MACHINE_MICROBLAZE      = 189,
    ELF_MACHINE_TILEGX          = 191,
    ELF_MACHINE_ARCV2           = 195,
    ELF_MACHINE_RISCV           = 243,
    ELF_MACHINE_BPF             = 247,
    ELF_MACHINE_CSKY            = 252,
    ELF_MACHINE_FRV             = 0x5441,

    ELF_MACHINE_ALPHA           = 0x9041,
    ELF_MACHINE_S390_OLD        = 0xa390,
    ELF_MACHINE_CYGNUS_MN10300  = 0xbeef,
};

#define EI_NIDENT   16

struct elf32_hdr {
    char        e_ident[EI_NIDENT]; /* ELF "magic number" */
    Elf32_Half  e_type;
    Elf32_Half  e_machine;
    Elf32_Word  e_version;
    Elf32_Addr  e_entry;        /* Entry point virtual address */
    Elf32_Off   e_phoff;        /* Program header table file offset */
    Elf32_Off   e_shoff;        /* Section header table file offset */
    Elf32_Word  e_flags;
    Elf32_Half  e_ehsize;
    Elf32_Half  e_phentsize;
    Elf32_Half  e_phnum;
    Elf32_Half  e_shentsize;
    Elf32_Half  e_shnum;
    Elf32_Half  e_shstrndx;
};

struct elf64_hdr {
    char        e_ident[EI_NIDENT]; /* ELF "magic number" */
    Elf64_Half  e_type;
    Elf64_Half  e_machine;
    Elf64_Word  e_version;
    Elf64_Addr  e_entry;        /* Entry point virtual address */
    Elf64_Off   e_phoff;        /* Program header table file offset */
    Elf64_Off   e_shoff;        /* Section header table file offset */
    Elf64_Word  e_flags;
    Elf64_Half  e_ehsize;
    Elf64_Half  e_phentsize;
    Elf64_Half  e_phnum;
    Elf64_Half  e_shentsize;
    Elf64_Half  e_shnum;
    Elf64_Half  e_shstrndx;
};

struct elf32_phdr{
    Elf32_Word  p_type;
    Elf32_Off   p_offset;       /* Segment file offset */
    Elf32_Addr  p_vaddr;        /* Segment virtual address */
    Elf32_Addr  p_paddr;        /* Segment physical address */
    Elf32_Word  p_filesz;       /* Segment size in file */
    Elf32_Word  p_memsz;        /* Segment size in memory */
    Elf32_Word  p_flags;
    Elf32_Word  p_align;        /* Segment alignment, file & memory */
};

struct elf64_phdr {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;       /* Segment file offset */
    Elf64_Addr  p_vaddr;        /* Segment virtual address */
    Elf64_Addr  p_paddr;        /* Segment physical address */
    Elf64_Xword p_filesz;       /* Segment size in file */
    Elf64_Xword p_memsz;        /* Segment size in memory */
    Elf64_Xword p_align;        /* Segment alignment, file & memory */
};

struct elf32_shdr {
    Elf32_Word  sh_name;        /* Section name, index in string tbl */
    Elf32_Word  sh_type;        /* Type of section */
    Elf32_Word  sh_flags;       /* Miscellaneous section attributes */
    Elf32_Addr  sh_addr;        /* Section virtual addr at execution */
    Elf32_Off   sh_offset;      /* Section file offset */
    Elf32_Word  sh_size;        /* Size of section in bytes */
    Elf32_Word  sh_link;        /* Index of another section */
    Elf32_Word  sh_info;        /* Additional section information */
    Elf32_Word  sh_addralign;   /* Section alignment */
    Elf32_Word  sh_entsize;     /* Entry size if section holds table */
};

struct elf64_shdr {
    Elf64_Word  sh_name;        /* Section name, index in string tbl */
    Elf64_Word  sh_type;        /* Type of section */
    Elf64_Xword sh_flags;       /* Miscellaneous section attributes */
    Elf64_Addr  sh_addr;        /* Section virtual addr at execution */
    Elf64_Off   sh_offset;      /* Section file offset */
    Elf64_Xword sh_size;        /* Size of section in bytes */
    Elf64_Word  sh_link;        /* Index of another section */
    Elf64_Word  sh_info;        /* Additional section information */
    Elf64_Xword sh_addralign;   /* Section alignment */
    Elf64_Xword sh_entsize;     /* Entry size if section holds table */
};

#endif  /* _LIGHTCORE_ELF_H_ */
