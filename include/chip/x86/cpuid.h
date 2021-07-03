#ifndef _CHIP_X86_CPUID_H_ 
#define _CHIP_X86_CPUID_H_


typedef struct cpuid_feat
{
    uint32_t fpu:1;     // bit 0
    uint32_t vme:1;     // bit 1
    uint32_t de:1;      // bit 2
    uint32_t pse:1;             // bit 3
    uint32_t tsc:1;             // bit 4
    uint32_t msr:1;             // bit 5
    uint32_t pae:1;             // bit 6
    uint32_t mce:1;             // bit 7
    uint32_t chg8b:1;   // bit 8
    uint32_t apic:1;    // bit 9
    uint32_t UDF0:1;    // bit 10
    uint32_t sys:1;     // bit 11
    uint32_t memtype:1; // bit 12
    uint32_t pge:1;     // bit 13
    uint32_t mca:1;     // bit 14
    uint32_t cmi:1;     // bit 15
    uint32_t pat:1;     // bit 16
    uint32_t UDF1:6;    // bit 17
    uint32_t mmx:1;     // bit 23
    uint32_t sse:1;     // bit 24
} cpuid_feat_t;


#endif
