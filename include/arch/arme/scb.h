#ifndef _ARCH_ARME_SCB_H_
#define _ARCH_ARME_SCB_H_

#include <types.h>
#include <arch/arme/map.h>

typedef struct
{
     uint32_t CPUID;                   /*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
     uint32_t ICSR;                    /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
     uint32_t VTOR;                    /*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
     uint32_t AIRCR;                   /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
     uint32_t SCR;                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
     uint32_t CCR;                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
     uint8_t  SHPR[12];                /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
     uint32_t SHCSR;                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
     uint32_t CFSR;                    /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
     uint32_t HFSR;                    /*!< Offset: 0x02C (R/W)  HardFault Status Register                             */
     uint32_t DFSR;                    /*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
     uint32_t MMFAR;                   /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register                      */
     uint32_t BFAR;                    /*!< Offset: 0x038 (R/W)  BusFault Address Register                             */
     uint32_t AFSR;                    /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
     uint32_t ID_PFR[2];               /*!< Offset: 0x040 (R/ )  Processor Feature Register                            */
     uint32_t ID_DFR;                  /*!< Offset: 0x048 (R/ )  Debug Feature Register                                */
     uint32_t ID_AFR;                  /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register                            */
     uint32_t ID_MFR[4];               /*!< Offset: 0x050 (R/ )  Memory Model Feature Register                         */
     uint32_t ID_ISAR[5];              /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register                   */
     uint32_t RESERVED0[1];
     uint32_t CLIDR;                   /*!< Offset: 0x078 (R/ )  Cache Level ID register                               */
     uint32_t CTR;                     /*!< Offset: 0x07C (R/ )  Cache Type register                                   */
     uint32_t CCSIDR;                  /*!< Offset: 0x080 (R/ )  Cache Size ID Register                                */
     uint32_t CSSELR;                  /*!< Offset: 0x084 (R/W)  Cache Size Selection Register                         */
     uint32_t CPACR;                   /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register                   */
     uint32_t RESERVED3[93];
     uint32_t STIR;                    /*!< Offset: 0x200 ( /W)  Software Triggered Interrupt Register                 */
     uint32_t RESERVED4[15];
     uint32_t MVFR0;                   /*!< Offset: 0x240 (R/ )  Media and VFP Feature Register 0                      */
     uint32_t MVFR1;                   /*!< Offset: 0x244 (R/ )  Media and VFP Feature Register 1                      */
     uint32_t MVFR2;                   /*!< Offset: 0x248 (R/ )  Media and VFP Feature Register 1                      */
     uint32_t RESERVED5[1];
     uint32_t ICIALLU;                 /*!< Offset: 0x250 ( /W)  I-Cache Invalidate All to PoU                         */
     uint32_t RESERVED6[1];
     uint32_t ICIMVAU;                 /*!< Offset: 0x258 ( /W)  I-Cache Invalidate by MVA to PoU                      */
     uint32_t DCIMVAC;                 /*!< Offset: 0x25C ( /W)  D-Cache Invalidate by MVA to PoC                      */
     uint32_t DCISW;                   /*!< Offset: 0x260 ( /W)  D-Cache Invalidate by Set-way                         */
     uint32_t DCCMVAU;                 /*!< Offset: 0x264 ( /W)  D-Cache Clean by MVA to PoU                           */
     uint32_t DCCMVAC;                 /*!< Offset: 0x268 ( /W)  D-Cache Clean by MVA to PoC                           */
     uint32_t DCCSW;                   /*!< Offset: 0x26C ( /W)  D-Cache Clean by Set-way                              */
     uint32_t DCCIMVAC;                /*!< Offset: 0x270 ( /W)  D-Cache Clean and Invalidate by MVA to PoC            */
     uint32_t DCCISW;                  /*!< Offset: 0x274 ( /W)  D-Cache Clean and Invalidate by Set-way               */
     uint32_t RESERVED7[6];
     uint32_t ITCMCR;                  /*!< Offset: 0x290 (R/W)  Instruction Tightly-Coupled Memory Control Register   */
     uint32_t DTCMCR;                  /*!< Offset: 0x294 (R/W)  Data Tightly-Coupled Memory Control Registers         */
     uint32_t AHBPCR;                  /*!< Offset: 0x298 (R/W)  AHBP Control Register                                 */
     uint32_t CACR;                    /*!< Offset: 0x29C (R/W)  L1 Cache Control Register                             */
     uint32_t AHBSCR;                  /*!< Offset: 0x2A0 (R/W)  AHB Slave Control Register                            */
     uint32_t RESERVED8[1];
     uint32_t ABFSR;                   /*!< Offset: 0x2A8 (R/W)  Auxiliary Bus Fault Status Register                   */
} SCB_Type;

#define SCB    ((SCB_Type *)SCB_BASE)

/* SCB CPUID Register Definitions */
#define SCB_CPUID_IMPLEMENTER_Pos          24                                             /*!< SCB CPUID: IMPLEMENTER Position */
#define SCB_CPUID_IMPLEMENTER_Msk          (0xFFUL << SCB_CPUID_IMPLEMENTER_Pos)          /*!< SCB CPUID: IMPLEMENTER Mask */

#define SCB_CPUID_VARIANT_Pos              20                                             /*!< SCB CPUID: VARIANT Position */
#define SCB_CPUID_VARIANT_Msk              (0xFUL << SCB_CPUID_VARIANT_Pos)               /*!< SCB CPUID: VARIANT Mask */

#define SCB_CPUID_ARCHITECTURE_Pos         16                                             /*!< SCB CPUID: ARCHITECTURE Position */
#define SCB_CPUID_ARCHITECTURE_Msk         (0xFUL << SCB_CPUID_ARCHITECTURE_Pos)          /*!< SCB CPUID: ARCHITECTURE Mask */

#define SCB_CPUID_PARTNO_Pos                4                                             /*!< SCB CPUID: PARTNO Position */
#define SCB_CPUID_PARTNO_Msk               (0xFFFUL << SCB_CPUID_PARTNO_Pos)              /*!< SCB CPUID: PARTNO Mask */

#define SCB_CPUID_REVISION_Pos              0                                             /*!< SCB CPUID: REVISION Position */
#define SCB_CPUID_REVISION_Msk             (0xFUL /*<< SCB_CPUID_REVISION_Pos*/)          /*!< SCB CPUID: REVISION Mask */

/* SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_NMIPENDSET_Pos            31                                             /*!< SCB ICSR: NMIPENDSET Position */
#define SCB_ICSR_NMIPENDSET_Msk            (1UL << SCB_ICSR_NMIPENDSET_Pos)               /*!< SCB ICSR: NMIPENDSET Mask */

#define SCB_ICSR_PENDSVSET_Pos             28                                             /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_Msk             (1UL << SCB_ICSR_PENDSVSET_Pos)                /*!< SCB ICSR: PENDSVSET Mask */

#define SCB_ICSR_PENDSVCLR_Pos             27                                             /*!< SCB ICSR: PENDSVCLR Position */
#define SCB_ICSR_PENDSVCLR_Msk             (1UL << SCB_ICSR_PENDSVCLR_Pos)                /*!< SCB ICSR: PENDSVCLR Mask */

#define SCB_ICSR_PENDSTSET_Pos             26                                             /*!< SCB ICSR: PENDSTSET Position */
#define SCB_ICSR_PENDSTSET_Msk             (1UL << SCB_ICSR_PENDSTSET_Pos)                /*!< SCB ICSR: PENDSTSET Mask */

#define SCB_ICSR_PENDSTCLR_Pos             25                                             /*!< SCB ICSR: PENDSTCLR Position */
#define SCB_ICSR_PENDSTCLR_Msk             (1UL << SCB_ICSR_PENDSTCLR_Pos)                /*!< SCB ICSR: PENDSTCLR Mask */

#define SCB_ICSR_ISRPREEMPT_Pos            23                                             /*!< SCB ICSR: ISRPREEMPT Position */
#define SCB_ICSR_ISRPREEMPT_Msk            (1UL << SCB_ICSR_ISRPREEMPT_Pos)               /*!< SCB ICSR: ISRPREEMPT Mask */

#define SCB_ICSR_ISRPENDING_Pos            22                                             /*!< SCB ICSR: ISRPENDING Position */
#define SCB_ICSR_ISRPENDING_Msk            (1UL << SCB_ICSR_ISRPENDING_Pos)               /*!< SCB ICSR: ISRPENDING Mask */

#define SCB_ICSR_VECTPENDING_Pos           12                                             /*!< SCB ICSR: VECTPENDING Position */
#define SCB_ICSR_VECTPENDING_Msk           (0x1FFUL << SCB_ICSR_VECTPENDING_Pos)          /*!< SCB ICSR: VECTPENDING Mask */

#define SCB_ICSR_RETTOBASE_Pos             11                                             /*!< SCB ICSR: RETTOBASE Position */
#define SCB_ICSR_RETTOBASE_Msk             (1UL << SCB_ICSR_RETTOBASE_Pos)                /*!< SCB ICSR: RETTOBASE Mask */

#define SCB_ICSR_VECTACTIVE_Pos             0                                             /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_Pos*/)       /*!< SCB ICSR: VECTACTIVE Mask */

/* SCB Vector Table Offset Register Definitions */
#define SCB_VTOR_TBLOFF_Pos                 7                                             /*!< SCB VTOR: TBLOFF Position */
#define SCB_VTOR_TBLOFF_Msk                (0x1FFFFFFUL << SCB_VTOR_TBLOFF_Pos)           /*!< SCB VTOR: TBLOFF Mask */

/* SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_Pos              16                                             /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_Pos          16                                             /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_Msk          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_Pos)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANESS_Pos            15                                             /*!< SCB AIRCR: ENDIANESS Position */
#define SCB_AIRCR_ENDIANESS_Msk            (1UL << SCB_AIRCR_ENDIANESS_Pos)               /*!< SCB AIRCR: ENDIANESS Mask */

#define SCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */

#define SCB_AIRCR_SYSRESETREQ_Pos           2                                             /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_Msk          (1UL << SCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_Pos         1                                             /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_Msk        (1UL << SCB_AIRCR_VECTCLRACTIVE_Pos)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

#define SCB_AIRCR_VECTRESET_Pos             0                                             /*!< SCB AIRCR: VECTRESET Position */
#define SCB_AIRCR_VECTRESET_Msk            (1UL /*<< SCB_AIRCR_VECTRESET_Pos*/)           /*!< SCB AIRCR: VECTRESET Mask */

/* SCB System Control Register Definitions */
#define SCB_SCR_SEVONPEND_Pos               4                                             /*!< SCB SCR: SEVONPEND Position */
#define SCB_SCR_SEVONPEND_Msk              (1UL << SCB_SCR_SEVONPEND_Pos)                 /*!< SCB SCR: SEVONPEND Mask */

#define SCB_SCR_SLEEPDEEP_Pos               2                                             /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_Msk              (1UL << SCB_SCR_SLEEPDEEP_Pos)                 /*!< SCB SCR: SLEEPDEEP Mask */

#define SCB_SCR_SLEEPONEXIT_Pos             1                                             /*!< SCB SCR: SLEEPONEXIT Position */
#define SCB_SCR_SLEEPONEXIT_Msk            (1UL << SCB_SCR_SLEEPONEXIT_Pos)               /*!< SCB SCR: SLEEPONEXIT Mask */

/* SCB Configuration Control Register Definitions */
#define SCB_CCR_BP_Pos                      18                                            /*!< SCB CCR: Branch prediction enable bit Position */
#define SCB_CCR_BP_Msk                     (1UL << SCB_CCR_BP_Pos)                        /*!< SCB CCR: Branch prediction enable bit Mask */

#define SCB_CCR_IC_Pos                      17                                            /*!< SCB CCR: Instruction cache enable bit Position */
#define SCB_CCR_IC_Msk                     (1UL << SCB_CCR_IC_Pos)                        /*!< SCB CCR: Instruction cache enable bit Mask */

#define SCB_CCR_DC_Pos                      16                                            /*!< SCB CCR: Cache enable bit Position */
#define SCB_CCR_DC_Msk                     (1UL << SCB_CCR_DC_Pos)                        /*!< SCB CCR: Cache enable bit Mask */

#define SCB_CCR_STKALIGN_Pos                9                                             /*!< SCB CCR: STKALIGN Position */
#define SCB_CCR_STKALIGN_Msk               (1UL << SCB_CCR_STKALIGN_Pos)                  /*!< SCB CCR: STKALIGN Mask */

#define SCB_CCR_BFHFNMIGN_Pos               8                                             /*!< SCB CCR: BFHFNMIGN Position */
#define SCB_CCR_BFHFNMIGN_Msk              (1UL << SCB_CCR_BFHFNMIGN_Pos)                 /*!< SCB CCR: BFHFNMIGN Mask */

#define SCB_CCR_DIV_0_TRP_Pos               4                                             /*!< SCB CCR: DIV_0_TRP Position */
#define SCB_CCR_DIV_0_TRP_Msk              (1UL << SCB_CCR_DIV_0_TRP_Pos)                 /*!< SCB CCR: DIV_0_TRP Mask */

#define SCB_CCR_UNALIGN_TRP_Pos             3                                             /*!< SCB CCR: UNALIGN_TRP Position */
#define SCB_CCR_UNALIGN_TRP_Msk            (1UL << SCB_CCR_UNALIGN_TRP_Pos)               /*!< SCB CCR: UNALIGN_TRP Mask */

#define SCB_CCR_USERSETMPEND_Pos            1                                             /*!< SCB CCR: USERSETMPEND Position */
#define SCB_CCR_USERSETMPEND_Msk           (1UL << SCB_CCR_USERSETMPEND_Pos)              /*!< SCB CCR: USERSETMPEND Mask */

#define SCB_CCR_NONBASETHRDENA_Pos          0                                             /*!< SCB CCR: NONBASETHRDENA Position */
#define SCB_CCR_NONBASETHRDENA_Msk         (1UL /*<< SCB_CCR_NONBASETHRDENA_Pos*/)        /*!< SCB CCR: NONBASETHRDENA Mask */

/* SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_USGFAULTENA_Pos          18                                             /*!< SCB SHCSR: USGFAULTENA Position */
#define SCB_SHCSR_USGFAULTENA_Msk          (1UL << SCB_SHCSR_USGFAULTENA_Pos)             /*!< SCB SHCSR: USGFAULTENA Mask */

#define SCB_SHCSR_BUSFAULTENA_Pos          17                                             /*!< SCB SHCSR: BUSFAULTENA Position */
#define SCB_SHCSR_BUSFAULTENA_Msk          (1UL << SCB_SHCSR_BUSFAULTENA_Pos)             /*!< SCB SHCSR: BUSFAULTENA Mask */

#define SCB_SHCSR_MEMFAULTENA_Pos          16                                             /*!< SCB SHCSR: MEMFAULTENA Position */
#define SCB_SHCSR_MEMFAULTENA_Msk          (1UL << SCB_SHCSR_MEMFAULTENA_Pos)             /*!< SCB SHCSR: MEMFAULTENA Mask */

#define SCB_SHCSR_SVCALLPENDED_Pos         15                                             /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_Msk         (1UL << SCB_SHCSR_SVCALLPENDED_Pos)            /*!< SCB SHCSR: SVCALLPENDED Mask */

#define SCB_SHCSR_BUSFAULTPENDED_Pos       14                                             /*!< SCB SHCSR: BUSFAULTPENDED Position */
#define SCB_SHCSR_BUSFAULTPENDED_Msk       (1UL << SCB_SHCSR_BUSFAULTPENDED_Pos)          /*!< SCB SHCSR: BUSFAULTPENDED Mask */

#define SCB_SHCSR_MEMFAULTPENDED_Pos       13                                             /*!< SCB SHCSR: MEMFAULTPENDED Position */
#define SCB_SHCSR_MEMFAULTPENDED_Msk       (1UL << SCB_SHCSR_MEMFAULTPENDED_Pos)          /*!< SCB SHCSR: MEMFAULTPENDED Mask */

#define SCB_SHCSR_USGFAULTPENDED_Pos       12                                             /*!< SCB SHCSR: USGFAULTPENDED Position */
#define SCB_SHCSR_USGFAULTPENDED_Msk       (1UL << SCB_SHCSR_USGFAULTPENDED_Pos)          /*!< SCB SHCSR: USGFAULTPENDED Mask */

#define SCB_SHCSR_SYSTICKACT_Pos           11                                             /*!< SCB SHCSR: SYSTICKACT Position */
#define SCB_SHCSR_SYSTICKACT_Msk           (1UL << SCB_SHCSR_SYSTICKACT_Pos)              /*!< SCB SHCSR: SYSTICKACT Mask */

#define SCB_SHCSR_PENDSVACT_Pos            10                                             /*!< SCB SHCSR: PENDSVACT Position */
#define SCB_SHCSR_PENDSVACT_Msk            (1UL << SCB_SHCSR_PENDSVACT_Pos)               /*!< SCB SHCSR: PENDSVACT Mask */

#define SCB_SHCSR_MONITORACT_Pos            8                                             /*!< SCB SHCSR: MONITORACT Position */
#define SCB_SHCSR_MONITORACT_Msk           (1UL << SCB_SHCSR_MONITORACT_Pos)              /*!< SCB SHCSR: MONITORACT Mask */

#define SCB_SHCSR_SVCALLACT_Pos             7                                             /*!< SCB SHCSR: SVCALLACT Position */
#define SCB_SHCSR_SVCALLACT_Msk            (1UL << SCB_SHCSR_SVCALLACT_Pos)               /*!< SCB SHCSR: SVCALLACT Mask */

#define SCB_SHCSR_USGFAULTACT_Pos           3                                             /*!< SCB SHCSR: USGFAULTACT Position */
#define SCB_SHCSR_USGFAULTACT_Msk          (1UL << SCB_SHCSR_USGFAULTACT_Pos)             /*!< SCB SHCSR: USGFAULTACT Mask */

#define SCB_SHCSR_BUSFAULTACT_Pos           1                                             /*!< SCB SHCSR: BUSFAULTACT Position */
#define SCB_SHCSR_BUSFAULTACT_Msk          (1UL << SCB_SHCSR_BUSFAULTACT_Pos)             /*!< SCB SHCSR: BUSFAULTACT Mask */

#define SCB_SHCSR_MEMFAULTACT_Pos           0                                             /*!< SCB SHCSR: MEMFAULTACT Position */
#define SCB_SHCSR_MEMFAULTACT_Msk          (1UL /*<< SCB_SHCSR_MEMFAULTACT_Pos*/)         /*!< SCB SHCSR: MEMFAULTACT Mask */

/* SCB Configurable Fault Status Registers Definitions */
#define SCB_CFSR_USGFAULTSR_Pos            16                                             /*!< SCB CFSR: Usage Fault Status Register Position */
#define SCB_CFSR_USGFAULTSR_Msk            (0xFFFFUL << SCB_CFSR_USGFAULTSR_Pos)          /*!< SCB CFSR: Usage Fault Status Register Mask */

#define SCB_CFSR_BUSFAULTSR_Pos             8                                             /*!< SCB CFSR: Bus Fault Status Register Position */
#define SCB_CFSR_BUSFAULTSR_Msk            (0xFFUL << SCB_CFSR_BUSFAULTSR_Pos)            /*!< SCB CFSR: Bus Fault Status Register Mask */

#define SCB_CFSR_MEMFAULTSR_Pos             0                                             /*!< SCB CFSR: Memory Manage Fault Status Register Position */
#define SCB_CFSR_MEMFAULTSR_Msk            (0xFFUL /*<< SCB_CFSR_MEMFAULTSR_Pos*/)        /*!< SCB CFSR: Memory Manage Fault Status Register Mask */

/* SCB Hard Fault Status Registers Definitions */
#define SCB_HFSR_DEBUGEVT_Pos              31                                             /*!< SCB HFSR: DEBUGEVT Position */
#define SCB_HFSR_DEBUGEVT_Msk              (1UL << SCB_HFSR_DEBUGEVT_Pos)                 /*!< SCB HFSR: DEBUGEVT Mask */

#define SCB_HFSR_FORCED_Pos                30                                             /*!< SCB HFSR: FORCED Position */
#define SCB_HFSR_FORCED_Msk                (1UL << SCB_HFSR_FORCED_Pos)                   /*!< SCB HFSR: FORCED Mask */

#define SCB_HFSR_VECTTBL_Pos                1                                             /*!< SCB HFSR: VECTTBL Position */
#define SCB_HFSR_VECTTBL_Msk               (1UL << SCB_HFSR_VECTTBL_Pos)                  /*!< SCB HFSR: VECTTBL Mask */

/* SCB Debug Fault Status Register Definitions */
#define SCB_DFSR_EXTERNAL_Pos               4                                             /*!< SCB DFSR: EXTERNAL Position */
#define SCB_DFSR_EXTERNAL_Msk              (1UL << SCB_DFSR_EXTERNAL_Pos)                 /*!< SCB DFSR: EXTERNAL Mask */

#define SCB_DFSR_VCATCH_Pos                 3                                             /*!< SCB DFSR: VCATCH Position */
#define SCB_DFSR_VCATCH_Msk                (1UL << SCB_DFSR_VCATCH_Pos)                   /*!< SCB DFSR: VCATCH Mask */

#define SCB_DFSR_DWTTRAP_Pos                2                                             /*!< SCB DFSR: DWTTRAP Position */
#define SCB_DFSR_DWTTRAP_Msk               (1UL << SCB_DFSR_DWTTRAP_Pos)                  /*!< SCB DFSR: DWTTRAP Mask */

#define SCB_DFSR_BKPT_Pos                   1                                             /*!< SCB DFSR: BKPT Position */
#define SCB_DFSR_BKPT_Msk                  (1UL << SCB_DFSR_BKPT_Pos)                     /*!< SCB DFSR: BKPT Mask */

#define SCB_DFSR_HALTED_Pos                 0                                             /*!< SCB DFSR: HALTED Position */
#define SCB_DFSR_HALTED_Msk                (1UL /*<< SCB_DFSR_HALTED_Pos*/)               /*!< SCB DFSR: HALTED Mask */

/* Cache Level ID register */
#define SCB_CLIDR_LOUU_Pos                 27                                             /*!< SCB CLIDR: LoUU Position */
#define SCB_CLIDR_LOUU_Msk                 (7UL << SCB_CLIDR_LOUU_Pos)                    /*!< SCB CLIDR: LoUU Mask */

#define SCB_CLIDR_LOC_Pos                  24                                             /*!< SCB CLIDR: LoC Position */
#define SCB_CLIDR_LOC_Msk                  (7UL << SCB_CLIDR_FORMAT_Pos)                  /*!< SCB CLIDR: LoC Mask */

/* Cache Type register */
#define SCB_CTR_FORMAT_Pos                 29                                             /*!< SCB CTR: Format Position */
#define SCB_CTR_FORMAT_Msk                 (7UL << SCB_CTR_FORMAT_Pos)                    /*!< SCB CTR: Format Mask */

#define SCB_CTR_CWG_Pos                    24                                             /*!< SCB CTR: CWG Position */
#define SCB_CTR_CWG_Msk                    (0xFUL << SCB_CTR_CWG_Pos)                     /*!< SCB CTR: CWG Mask */

#define SCB_CTR_ERG_Pos                    20                                             /*!< SCB CTR: ERG Position */
#define SCB_CTR_ERG_Msk                    (0xFUL << SCB_CTR_ERG_Pos)                     /*!< SCB CTR: ERG Mask */

#define SCB_CTR_DMINLINE_Pos               16                                             /*!< SCB CTR: DminLine Position */
#define SCB_CTR_DMINLINE_Msk               (0xFUL << SCB_CTR_DMINLINE_Pos)                /*!< SCB CTR: DminLine Mask */

#define SCB_CTR_IMINLINE_Pos                0                                             /*!< SCB CTR: ImInLine Position */
#define SCB_CTR_IMINLINE_Msk               (0xFUL /*<< SCB_CTR_IMINLINE_Pos*/)            /*!< SCB CTR: ImInLine Mask */

/* Cache Size ID Register */
#define SCB_CCSIDR_WT_Pos                  31                                             /*!< SCB CCSIDR: WT Position */
#define SCB_CCSIDR_WT_Msk                  (7UL << SCB_CCSIDR_WT_Pos)                     /*!< SCB CCSIDR: WT Mask */

#define SCB_CCSIDR_WB_Pos                  30                                             /*!< SCB CCSIDR: WB Position */
#define SCB_CCSIDR_WB_Msk                  (7UL << SCB_CCSIDR_WB_Pos)                     /*!< SCB CCSIDR: WB Mask */

#define SCB_CCSIDR_RA_Pos                  29                                             /*!< SCB CCSIDR: RA Position */
#define SCB_CCSIDR_RA_Msk                  (7UL << SCB_CCSIDR_RA_Pos)                     /*!< SCB CCSIDR: RA Mask */

#define SCB_CCSIDR_WA_Pos                  28                                             /*!< SCB CCSIDR: WA Position */
#define SCB_CCSIDR_WA_Msk                  (7UL << SCB_CCSIDR_WA_Pos)                     /*!< SCB CCSIDR: WA Mask */

#define SCB_CCSIDR_NUMSETS_Pos             13                                             /*!< SCB CCSIDR: NumSets Position */
#define SCB_CCSIDR_NUMSETS_Msk             (0x7FFFUL << SCB_CCSIDR_NUMSETS_Pos)           /*!< SCB CCSIDR: NumSets Mask */

#define SCB_CCSIDR_ASSOCIATIVITY_Pos        3                                             /*!< SCB CCSIDR: Associativity Position */
#define SCB_CCSIDR_ASSOCIATIVITY_Msk       (0x3FFUL << SCB_CCSIDR_ASSOCIATIVITY_Pos)      /*!< SCB CCSIDR: Associativity Mask */

#define SCB_CCSIDR_LINESIZE_Pos             0                                             /*!< SCB CCSIDR: LineSize Position */
#define SCB_CCSIDR_LINESIZE_Msk            (7UL /*<< SCB_CCSIDR_LINESIZE_Pos*/)           /*!< SCB CCSIDR: LineSize Mask */

/* Cache Size Selection Register */
#define SCB_CSSELR_LEVEL_Pos                1                                             /*!< SCB CSSELR: Level Position */
#define SCB_CSSELR_LEVEL_Msk               (7UL << SCB_CSSELR_LEVEL_Pos)                  /*!< SCB CSSELR: Level Mask */

#define SCB_CSSELR_IND_Pos                  0                                             /*!< SCB CSSELR: InD Position */
#define SCB_CSSELR_IND_Msk                 (1UL /*<< SCB_CSSELR_IND_Pos*/)                /*!< SCB CSSELR: InD Mask */

/* SCB Software Triggered Interrupt Register */
#define SCB_STIR_INTID_Pos                  0                                             /*!< SCB STIR: INTID Position */
#define SCB_STIR_INTID_Msk                 (0x1FFUL /*<< SCB_STIR_INTID_Pos*/)            /*!< SCB STIR: INTID Mask */

/* Instruction Tightly-Coupled Memory Control Register*/
#define SCB_ITCMCR_SZ_Pos                   3                                             /*!< SCB ITCMCR: SZ Position */
#define SCB_ITCMCR_SZ_Msk                  (0xFUL << SCB_ITCMCR_SZ_Pos)                   /*!< SCB ITCMCR: SZ Mask */

#define SCB_ITCMCR_RETEN_Pos                2                                             /*!< SCB ITCMCR: RETEN Position */
#define SCB_ITCMCR_RETEN_Msk               (1UL << SCB_ITCMCR_RETEN_Pos)                  /*!< SCB ITCMCR: RETEN Mask */

#define SCB_ITCMCR_RMW_Pos                  1                                             /*!< SCB ITCMCR: RMW Position */
#define SCB_ITCMCR_RMW_Msk                 (1UL << SCB_ITCMCR_RMW_Pos)                    /*!< SCB ITCMCR: RMW Mask */

#define SCB_ITCMCR_EN_Pos                   0                                             /*!< SCB ITCMCR: EN Position */
#define SCB_ITCMCR_EN_Msk                  (1UL /*<< SCB_ITCMCR_EN_Pos*/)                 /*!< SCB ITCMCR: EN Mask */

/* Data Tightly-Coupled Memory Control Registers */
#define SCB_DTCMCR_SZ_Pos                   3                                             /*!< SCB DTCMCR: SZ Position */
#define SCB_DTCMCR_SZ_Msk                  (0xFUL << SCB_DTCMCR_SZ_Pos)                   /*!< SCB DTCMCR: SZ Mask */

#define SCB_DTCMCR_RETEN_Pos                2                                             /*!< SCB DTCMCR: RETEN Position */
#define SCB_DTCMCR_RETEN_Msk               (1UL << SCB_DTCMCR_RETEN_Pos)                   /*!< SCB DTCMCR: RETEN Mask */

#define SCB_DTCMCR_RMW_Pos                  1                                             /*!< SCB DTCMCR: RMW Position */
#define SCB_DTCMCR_RMW_Msk                 (1UL << SCB_DTCMCR_RMW_Pos)                    /*!< SCB DTCMCR: RMW Mask */

#define SCB_DTCMCR_EN_Pos                   0                                             /*!< SCB DTCMCR: EN Position */
#define SCB_DTCMCR_EN_Msk                  (1UL /*<< SCB_DTCMCR_EN_Pos*/)                 /*!< SCB DTCMCR: EN Mask */

/* AHBP Control Register */
#define SCB_AHBPCR_SZ_Pos                   1                                             /*!< SCB AHBPCR: SZ Position */
#define SCB_AHBPCR_SZ_Msk                  (7UL << SCB_AHBPCR_SZ_Pos)                     /*!< SCB AHBPCR: SZ Mask */

#define SCB_AHBPCR_EN_Pos                   0                                             /*!< SCB AHBPCR: EN Position */
#define SCB_AHBPCR_EN_Msk                  (1UL /*<< SCB_AHBPCR_EN_Pos*/)                 /*!< SCB AHBPCR: EN Mask */

/* L1 Cache Control Register */
#define SCB_CACR_FORCEWT_Pos                2                                             /*!< SCB CACR: FORCEWT Position */
#define SCB_CACR_FORCEWT_Msk               (1UL << SCB_CACR_FORCEWT_Pos)                  /*!< SCB CACR: FORCEWT Mask */

#define SCB_CACR_ECCEN_Pos                  1                                             /*!< SCB CACR: ECCEN Position */
#define SCB_CACR_ECCEN_Msk                 (1UL << SCB_CACR_ECCEN_Pos)                    /*!< SCB CACR: ECCEN Mask */

#define SCB_CACR_SIWT_Pos                   0                                             /*!< SCB CACR: SIWT Position */
#define SCB_CACR_SIWT_Msk                  (1UL /*<< SCB_CACR_SIWT_Pos*/)                 /*!< SCB CACR: SIWT Mask */

/* AHBS control register */
#define SCB_AHBSCR_INITCOUNT_Pos           11                                             /*!< SCB AHBSCR: INITCOUNT Position */
#define SCB_AHBSCR_INITCOUNT_Msk           (0x1FUL << SCB_AHBPCR_INITCOUNT_Pos)           /*!< SCB AHBSCR: INITCOUNT Mask */

#define SCB_AHBSCR_TPRI_Pos                 2                                             /*!< SCB AHBSCR: TPRI Position */
#define SCB_AHBSCR_TPRI_Msk                (0x1FFUL << SCB_AHBPCR_TPRI_Pos)               /*!< SCB AHBSCR: TPRI Mask */

#define SCB_AHBSCR_CTL_Pos                  0                                             /*!< SCB AHBSCR: CTL Position*/
#define SCB_AHBSCR_CTL_Msk                 (3UL /*<< SCB_AHBPCR_CTL_Pos*/)                /*!< SCB AHBSCR: CTL Mask */

/* Auxiliary Bus Fault Status Register */
#define SCB_ABFSR_AXIMTYPE_Pos              8                                             /*!< SCB ABFSR: AXIMTYPE Position*/
#define SCB_ABFSR_AXIMTYPE_Msk             (3UL << SCB_ABFSR_AXIMTYPE_Pos)                /*!< SCB ABFSR: AXIMTYPE Mask */

#define SCB_ABFSR_EPPB_Pos                  4                                             /*!< SCB ABFSR: EPPB Position*/
#define SCB_ABFSR_EPPB_Msk                 (1UL << SCB_ABFSR_EPPB_Pos)                    /*!< SCB ABFSR: EPPB Mask */

#define SCB_ABFSR_AXIM_Pos                  3                                             /*!< SCB ABFSR: AXIM Position*/
#define SCB_ABFSR_AXIM_Msk                 (1UL << SCB_ABFSR_AXIM_Pos)                    /*!< SCB ABFSR: AXIM Mask */

#define SCB_ABFSR_AHBP_Pos                  2                                             /*!< SCB ABFSR: AHBP Position*/
#define SCB_ABFSR_AHBP_Msk                 (1UL << SCB_ABFSR_AHBP_Pos)                    /*!< SCB ABFSR: AHBP Mask */

#define SCB_ABFSR_DTCM_Pos                  1                                             /*!< SCB ABFSR: DTCM Position*/
#define SCB_ABFSR_DTCM_Msk                 (1UL << SCB_ABFSR_DTCM_Pos)                    /*!< SCB ABFSR: DTCM Mask */

#define SCB_ABFSR_ITCM_Pos                  0                                             /*!< SCB ABFSR: ITCM Position*/
#define SCB_ABFSR_ITCM_Msk                 (1UL /*<< SCB_ABFSR_ITCM_Pos*/)                /*!< SCB ABFSR: ITCM Mask */

/*@} end of group CMSIS_SCB */



#endif /* _ARCH_ARME_SCB_H_ */