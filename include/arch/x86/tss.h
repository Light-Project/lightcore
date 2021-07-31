#ifndef _ARCH_X86_TSS_H_
#define _ARCH_X86_TSS_H_

struct tss_entry {
    uint16_t link;      // LINK
    uint16_t RES0;      // Reserved
    uint32_t esp0;      // ESP0
    uint16_t ss0;       // SS0
    uint16_t RES1;      // Reserved
    uint32_t esp1;      // ESP1
    uint16_t ss1;       // SS1
    uint16_t RES2;      // Reserved
    uint32_t esp2;      // ESP2
    uint16_t ss2;       // SS2
    uint16_t RES3;      // RES3
    uint32_t cr3;       // CR3
    uint32_t eip;       // EIP
    uint32_t eflags;    // EFLAGS
    uint32_t eax;       // EAX 
    uint32_t ecx;       // ECX
    uint32_t edx;       // EDX
    uint32_t ebx;       // EBX
    uint32_t esp;       // ESP
    uint32_t ebp;       // EBP
    uint32_t esi;       // ESI
    uint32_t edi;       // EDI
    uint16_t es;        // ES
    uint16_t RES4;      // Reserved
    uint16_t cs;        // CS
    uint16_t RES5;      // Reserved
    uint16_t ss;        // SS
    uint16_t RES6;      // Reserved
    uint16_t ds;        // DS
    uint16_t RES7;      // Reserved
    uint16_t fs;        // FS
    uint16_t RES8;      // Reserved
    uint16_t gs;        // GS
    uint16_t RES9;      // Reserved
    uint16_t ldt_seg;   // ldt_seg
    uint16_t RES10;     // Reserved
    uint16_t RES11;     // Reserved
    uint16_t iobase;    // iobase
} __packed; 

#endif
