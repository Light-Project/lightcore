#ifndef _ARCH_ARME_TPI_H_
#define _ARCH_ARME_TPI_H_

#include <types.h>
#include <arch/arme/map.h>

typedef struct
{
    uint32_t SSPSR;                   /*!< Offset: 0x000 (R/ )  Supported Parallel Port Size Register     */
    uint32_t CSPSR;                   /*!< Offset: 0x004 (R/W)  Current Parallel Port Size Register */
    uint32_t RESERVED0[2];
    uint32_t ACPR;                    /*!< Offset: 0x010 (R/W)  Asynchronous Clock Prescaler Register */
    uint32_t RESERVED1[55];
    uint32_t SPPR;                    /*!< Offset: 0x0F0 (R/W)  Selected Pin Protocol Register */
    uint32_t RESERVED2[131];
    uint32_t FFSR;                    /*!< Offset: 0x300 (R/ )  Formatter and Flush Status Register */
    uint32_t FFCR;                    /*!< Offset: 0x304 (R/W)  Formatter and Flush Control Register */
    uint32_t FSCR;                    /*!< Offset: 0x308 (R/ )  Formatter Synchronization Counter Register */
    uint32_t RESERVED3[759];
    uint32_t TRIGGER;                 /*!< Offset: 0xEE8 (R/ )  TRIGGER */
    uint32_t FIFO0;                   /*!< Offset: 0xEEC (R/ )  Integration ETM Data */
    uint32_t ITATBCTR2;               /*!< Offset: 0xEF0 (R/ )  ITATBCTR2 */
    uint32_t RESERVED4[1];
    uint32_t ITATBCTR0;               /*!< Offset: 0xEF8 (R/ )  ITATBCTR0 */
    uint32_t FIFO1;                   /*!< Offset: 0xEFC (R/ )  Integration ITM Data */
    uint32_t ITCTRL;                  /*!< Offset: 0xF00 (R/W)  Integration Mode Control */
    uint32_t RESERVED5[39];
    uint32_t CLAIMSET;                /*!< Offset: 0xFA0 (R/W)  Claim tag set */
    uint32_t CLAIMCLR;                /*!< Offset: 0xFA4 (R/W)  Claim tag clear */
    uint32_t RESERVED7[8];
    uint32_t DEVID;                   /*!< Offset: 0xFC8 (R/ )  TPIU_DEVID */
    uint32_t DEVTYPE;                 /*!< Offset: 0xFCC (R/ )  TPIU_DEVTYPE */
} TPI_Type;

#define TPI     ((TPI_Type *)TPI_BASE)

/* TPI Asynchronous Clock Prescaler Register Definitions */
#define TPI_ACPR_PRESCALER_Pos              0                                          /*!< TPI ACPR: PRESCALER Position */
#define TPI_ACPR_PRESCALER_Msk             (0x1FFFUL /*<< TPI_ACPR_PRESCALER_Pos*/)    /*!< TPI ACPR: PRESCALER Mask */

/* TPI Selected Pin Protocol Register Definitions */
#define TPI_SPPR_TXMODE_Pos                 0                                          /*!< TPI SPPR: TXMODE Position */
#define TPI_SPPR_TXMODE_Msk                (0x3UL /*<< TPI_SPPR_TXMODE_Pos*/)          /*!< TPI SPPR: TXMODE Mask */

/* TPI Formatter and Flush Status Register Definitions */
#define TPI_FFSR_FtNonStop_Pos              3                                          /*!< TPI FFSR: FtNonStop Position */
#define TPI_FFSR_FtNonStop_Msk             (0x1UL << TPI_FFSR_FtNonStop_Pos)           /*!< TPI FFSR: FtNonStop Mask */

#define TPI_FFSR_TCPresent_Pos              2                                          /*!< TPI FFSR: TCPresent Position */
#define TPI_FFSR_TCPresent_Msk             (0x1UL << TPI_FFSR_TCPresent_Pos)           /*!< TPI FFSR: TCPresent Mask */

#define TPI_FFSR_FtStopped_Pos              1                                          /*!< TPI FFSR: FtStopped Position */
#define TPI_FFSR_FtStopped_Msk             (0x1UL << TPI_FFSR_FtStopped_Pos)           /*!< TPI FFSR: FtStopped Mask */

#define TPI_FFSR_FlInProg_Pos               0                                          /*!< TPI FFSR: FlInProg Position */
#define TPI_FFSR_FlInProg_Msk              (0x1UL /*<< TPI_FFSR_FlInProg_Pos*/)        /*!< TPI FFSR: FlInProg Mask */

/* TPI Formatter and Flush Control Register Definitions */
#define TPI_FFCR_TrigIn_Pos                 8                                          /*!< TPI FFCR: TrigIn Position */
#define TPI_FFCR_TrigIn_Msk                (0x1UL << TPI_FFCR_TrigIn_Pos)              /*!< TPI FFCR: TrigIn Mask */

#define TPI_FFCR_EnFCont_Pos                1                                          /*!< TPI FFCR: EnFCont Position */
#define TPI_FFCR_EnFCont_Msk               (0x1UL << TPI_FFCR_EnFCont_Pos)             /*!< TPI FFCR: EnFCont Mask */

/* TPI TRIGGER Register Definitions */
#define TPI_TRIGGER_TRIGGER_Pos             0                                          /*!< TPI TRIGGER: TRIGGER Position */
#define TPI_TRIGGER_TRIGGER_Msk            (0x1UL /*<< TPI_TRIGGER_TRIGGER_Pos*/)      /*!< TPI TRIGGER: TRIGGER Mask */

/* TPI Integration ETM Data Register Definitions (FIFO0) */
#define TPI_FIFO0_ITM_ATVALID_Pos          29                                          /*!< TPI FIFO0: ITM_ATVALID Position */
#define TPI_FIFO0_ITM_ATVALID_Msk          (0x3UL << TPI_FIFO0_ITM_ATVALID_Pos)        /*!< TPI FIFO0: ITM_ATVALID Mask */

#define TPI_FIFO0_ITM_bytecount_Pos        27                                          /*!< TPI FIFO0: ITM_bytecount Position */
#define TPI_FIFO0_ITM_bytecount_Msk        (0x3UL << TPI_FIFO0_ITM_bytecount_Pos)      /*!< TPI FIFO0: ITM_bytecount Mask */

#define TPI_FIFO0_ETM_ATVALID_Pos          26                                          /*!< TPI FIFO0: ETM_ATVALID Position */
#define TPI_FIFO0_ETM_ATVALID_Msk          (0x3UL << TPI_FIFO0_ETM_ATVALID_Pos)        /*!< TPI FIFO0: ETM_ATVALID Mask */

#define TPI_FIFO0_ETM_bytecount_Pos        24                                          /*!< TPI FIFO0: ETM_bytecount Position */
#define TPI_FIFO0_ETM_bytecount_Msk        (0x3UL << TPI_FIFO0_ETM_bytecount_Pos)      /*!< TPI FIFO0: ETM_bytecount Mask */

#define TPI_FIFO0_ETM2_Pos                 16                                          /*!< TPI FIFO0: ETM2 Position */
#define TPI_FIFO0_ETM2_Msk                 (0xFFUL << TPI_FIFO0_ETM2_Pos)              /*!< TPI FIFO0: ETM2 Mask */

#define TPI_FIFO0_ETM1_Pos                  8                                          /*!< TPI FIFO0: ETM1 Position */
#define TPI_FIFO0_ETM1_Msk                 (0xFFUL << TPI_FIFO0_ETM1_Pos)              /*!< TPI FIFO0: ETM1 Mask */

#define TPI_FIFO0_ETM0_Pos                  0                                          /*!< TPI FIFO0: ETM0 Position */
#define TPI_FIFO0_ETM0_Msk                 (0xFFUL /*<< TPI_FIFO0_ETM0_Pos*/)          /*!< TPI FIFO0: ETM0 Mask */

/* TPI ITATBCTR2 Register Definitions */
#define TPI_ITATBCTR2_ATREADY_Pos           0                                          /*!< TPI ITATBCTR2: ATREADY Position */
#define TPI_ITATBCTR2_ATREADY_Msk          (0x1UL /*<< TPI_ITATBCTR2_ATREADY_Pos*/)    /*!< TPI ITATBCTR2: ATREADY Mask */

/* TPI Integration ITM Data Register Definitions (FIFO1) */
#define TPI_FIFO1_ITM_ATVALID_Pos          29                                          /*!< TPI FIFO1: ITM_ATVALID Position */
#define TPI_FIFO1_ITM_ATVALID_Msk          (0x3UL << TPI_FIFO1_ITM_ATVALID_Pos)        /*!< TPI FIFO1: ITM_ATVALID Mask */

#define TPI_FIFO1_ITM_bytecount_Pos        27                                          /*!< TPI FIFO1: ITM_bytecount Position */
#define TPI_FIFO1_ITM_bytecount_Msk        (0x3UL << TPI_FIFO1_ITM_bytecount_Pos)      /*!< TPI FIFO1: ITM_bytecount Mask */

#define TPI_FIFO1_ETM_ATVALID_Pos          26                                          /*!< TPI FIFO1: ETM_ATVALID Position */
#define TPI_FIFO1_ETM_ATVALID_Msk          (0x3UL << TPI_FIFO1_ETM_ATVALID_Pos)        /*!< TPI FIFO1: ETM_ATVALID Mask */

#define TPI_FIFO1_ETM_bytecount_Pos        24                                          /*!< TPI FIFO1: ETM_bytecount Position */
#define TPI_FIFO1_ETM_bytecount_Msk        (0x3UL << TPI_FIFO1_ETM_bytecount_Pos)      /*!< TPI FIFO1: ETM_bytecount Mask */

#define TPI_FIFO1_ITM2_Pos                 16                                          /*!< TPI FIFO1: ITM2 Position */
#define TPI_FIFO1_ITM2_Msk                 (0xFFUL << TPI_FIFO1_ITM2_Pos)              /*!< TPI FIFO1: ITM2 Mask */

#define TPI_FIFO1_ITM1_Pos                  8                                          /*!< TPI FIFO1: ITM1 Position */
#define TPI_FIFO1_ITM1_Msk                 (0xFFUL << TPI_FIFO1_ITM1_Pos)              /*!< TPI FIFO1: ITM1 Mask */

#define TPI_FIFO1_ITM0_Pos                  0                                          /*!< TPI FIFO1: ITM0 Position */
#define TPI_FIFO1_ITM0_Msk                 (0xFFUL /*<< TPI_FIFO1_ITM0_Pos*/)          /*!< TPI FIFO1: ITM0 Mask */

/* TPI ITATBCTR0 Register Definitions */
#define TPI_ITATBCTR0_ATREADY_Pos           0                                          /*!< TPI ITATBCTR0: ATREADY Position */
#define TPI_ITATBCTR0_ATREADY_Msk          (0x1UL /*<< TPI_ITATBCTR0_ATREADY_Pos*/)    /*!< TPI ITATBCTR0: ATREADY Mask */

/* TPI Integration Mode Control Register Definitions */
#define TPI_ITCTRL_Mode_Pos                 0                                          /*!< TPI ITCTRL: Mode Position */
#define TPI_ITCTRL_Mode_Msk                (0x1UL /*<< TPI_ITCTRL_Mode_Pos*/)          /*!< TPI ITCTRL: Mode Mask */

/* TPI DEVID Register Definitions */
#define TPI_DEVID_NRZVALID_Pos             11                                          /*!< TPI DEVID: NRZVALID Position */
#define TPI_DEVID_NRZVALID_Msk             (0x1UL << TPI_DEVID_NRZVALID_Pos)           /*!< TPI DEVID: NRZVALID Mask */

#define TPI_DEVID_MANCVALID_Pos            10                                          /*!< TPI DEVID: MANCVALID Position */
#define TPI_DEVID_MANCVALID_Msk            (0x1UL << TPI_DEVID_MANCVALID_Pos)          /*!< TPI DEVID: MANCVALID Mask */

#define TPI_DEVID_PTINVALID_Pos             9                                          /*!< TPI DEVID: PTINVALID Position */
#define TPI_DEVID_PTINVALID_Msk            (0x1UL << TPI_DEVID_PTINVALID_Pos)          /*!< TPI DEVID: PTINVALID Mask */

#define TPI_DEVID_MinBufSz_Pos              6                                          /*!< TPI DEVID: MinBufSz Position */
#define TPI_DEVID_MinBufSz_Msk             (0x7UL << TPI_DEVID_MinBufSz_Pos)           /*!< TPI DEVID: MinBufSz Mask */

#define TPI_DEVID_AsynClkIn_Pos             5                                          /*!< TPI DEVID: AsynClkIn Position */
#define TPI_DEVID_AsynClkIn_Msk            (0x1UL << TPI_DEVID_AsynClkIn_Pos)          /*!< TPI DEVID: AsynClkIn Mask */

#define TPI_DEVID_NrTraceInput_Pos          0                                          /*!< TPI DEVID: NrTraceInput Position */
#define TPI_DEVID_NrTraceInput_Msk         (0x1FUL /*<< TPI_DEVID_NrTraceInput_Pos*/)  /*!< TPI DEVID: NrTraceInput Mask */

/* TPI DEVTYPE Register Definitions */
#define TPI_DEVTYPE_MajorType_Pos           4                                          /*!< TPI DEVTYPE: MajorType Position */
#define TPI_DEVTYPE_MajorType_Msk          (0xFUL << TPI_DEVTYPE_MajorType_Pos)        /*!< TPI DEVTYPE: MajorType Mask */

#define TPI_DEVTYPE_SubType_Pos             0                                          /*!< TPI DEVTYPE: SubType Position */
#define TPI_DEVTYPE_SubType_Msk            (0xFUL /*<< TPI_DEVTYPE_SubType_Pos*/)      /*!< TPI DEVTYPE: SubType Mask */
 

#endif /* _ARCH_ARME_TPI_H_ */
