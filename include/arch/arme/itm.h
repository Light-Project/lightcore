/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/arch/arme/itm.h
 *  arme instrumentation trace macrocell
 *  (C) 2020 arm
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-30      first version 
 * 
 */

#ifndef _ARCH_ARME_ITM_H_
#define _ARCH_ARME_ITM_H_

#include <types.h>
#include <arch/arme/map.h>

typedef struct
{
  union
  {
        uint8_t    u8;                  /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 8-bit                   */
        uint16_t   u16;                 /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 16-bit                  */
        uint32_t   u32;                 /*!< Offset: 0x000 ( /W)  ITM Stimulus Port 32-bit                  */
  }  PORT [32];                         /*!< Offset: 0x000 ( /W)  ITM Stimulus Port Registers               */
    uint32_t RESERVED0[864];
    uint32_t TER;                     /*!< Offset: 0xE00 (R/W)  ITM Trace Enable Register                 */
    uint32_t RESERVED1[15];
    uint32_t TPR;                     /*!< Offset: 0xE40 (R/W)  ITM Trace Privilege Register              */
    uint32_t RESERVED2[15];
    uint32_t TCR;                     /*!< Offset: 0xE80 (R/W)  ITM Trace Control Register                */
    uint32_t RESERVED3[29];
    uint32_t IWR;                     /*!< Offset: 0xEF8 ( /W)  ITM Integration Write Register            */
    uint32_t IRR;                     /*!< Offset: 0xEFC (R/ )  ITM Integration Read Register             */
    uint32_t IMCR;                    /*!< Offset: 0xF00 (R/W)  ITM Integration Mode Control Register     */
    uint32_t RESERVED4[43];
    uint32_t LAR;                     /*!< Offset: 0xFB0 ( /W)  ITM Lock Access Register                  */
    uint32_t LSR;                     /*!< Offset: 0xFB4 (R/ )  ITM Lock Status Register                  */
    uint32_t RESERVED5[6];
    uint32_t PID4;                    /*!< Offset: 0xFD0 (R/ )  ITM Peripheral Identification Register #4 */
    uint32_t PID5;                    /*!< Offset: 0xFD4 (R/ )  ITM Peripheral Identification Register #5 */
    uint32_t PID6;                    /*!< Offset: 0xFD8 (R/ )  ITM Peripheral Identification Register #6 */
    uint32_t PID7;                    /*!< Offset: 0xFDC (R/ )  ITM Peripheral Identification Register #7 */
    uint32_t PID0;                    /*!< Offset: 0xFE0 (R/ )  ITM Peripheral Identification Register #0 */
    uint32_t PID1;                    /*!< Offset: 0xFE4 (R/ )  ITM Peripheral Identification Register #1 */
    uint32_t PID2;                    /*!< Offset: 0xFE8 (R/ )  ITM Peripheral Identification Register #2 */
    uint32_t PID3;                    /*!< Offset: 0xFEC (R/ )  ITM Peripheral Identification Register #3 */
    uint32_t CID0;                    /*!< Offset: 0xFF0 (R/ )  ITM Component  Identification Register #0 */
    uint32_t CID1;                    /*!< Offset: 0xFF4 (R/ )  ITM Component  Identification Register #1 */
    uint32_t CID2;                    /*!< Offset: 0xFF8 (R/ )  ITM Component  Identification Register #2 */
    uint32_t CID3;                    /*!< Offset: 0xFFC (R/ )  ITM Component  Identification Register #3 */
} ITM_Type;

#define ITM     ((ITM_Type *)ITM_BASE)

/* ITM Trace Privilege Register Definitions */
#define ITM_TPR_PRIVMASK_Pos                0                                             /*!< ITM TPR: PRIVMASK Position */
#define ITM_TPR_PRIVMASK_Msk               (0xFUL /*<< ITM_TPR_PRIVMASK_Pos*/)            /*!< ITM TPR: PRIVMASK Mask */

/* ITM Trace Control Register Definitions */
#define ITM_TCR_BUSY_Pos                   23                                             /*!< ITM TCR: BUSY Position */
#define ITM_TCR_BUSY_Msk                   (1UL << ITM_TCR_BUSY_Pos)                      /*!< ITM TCR: BUSY Mask */

#define ITM_TCR_TraceBusID_Pos             16                                             /*!< ITM TCR: ATBID Position */
#define ITM_TCR_TraceBusID_Msk             (0x7FUL << ITM_TCR_TraceBusID_Pos)             /*!< ITM TCR: ATBID Mask */

#define ITM_TCR_GTSFREQ_Pos                10                                             /*!< ITM TCR: Global timestamp frequency Position */
#define ITM_TCR_GTSFREQ_Msk                (3UL << ITM_TCR_GTSFREQ_Pos)                   /*!< ITM TCR: Global timestamp frequency Mask */

#define ITM_TCR_TSPrescale_Pos              8                                             /*!< ITM TCR: TSPrescale Position */
#define ITM_TCR_TSPrescale_Msk             (3UL << ITM_TCR_TSPrescale_Pos)                /*!< ITM TCR: TSPrescale Mask */

#define ITM_TCR_SWOENA_Pos                  4                                             /*!< ITM TCR: SWOENA Position */
#define ITM_TCR_SWOENA_Msk                 (1UL << ITM_TCR_SWOENA_Pos)                    /*!< ITM TCR: SWOENA Mask */

#define ITM_TCR_DWTENA_Pos                  3                                             /*!< ITM TCR: DWTENA Position */
#define ITM_TCR_DWTENA_Msk                 (1UL << ITM_TCR_DWTENA_Pos)                    /*!< ITM TCR: DWTENA Mask */

#define ITM_TCR_SYNCENA_Pos                 2                                             /*!< ITM TCR: SYNCENA Position */
#define ITM_TCR_SYNCENA_Msk                (1UL << ITM_TCR_SYNCENA_Pos)                   /*!< ITM TCR: SYNCENA Mask */

#define ITM_TCR_TSENA_Pos                   1                                             /*!< ITM TCR: TSENA Position */
#define ITM_TCR_TSENA_Msk                  (1UL << ITM_TCR_TSENA_Pos)                     /*!< ITM TCR: TSENA Mask */

#define ITM_TCR_ITMENA_Pos                  0                                             /*!< ITM TCR: ITM Enable bit Position */
#define ITM_TCR_ITMENA_Msk                 (1UL /*<< ITM_TCR_ITMENA_Pos*/)                /*!< ITM TCR: ITM Enable bit Mask */

/* ITM Integration Write Register Definitions */
#define ITM_IWR_ATVALIDM_Pos                0                                             /*!< ITM IWR: ATVALIDM Position */
#define ITM_IWR_ATVALIDM_Msk               (1UL /*<< ITM_IWR_ATVALIDM_Pos*/)              /*!< ITM IWR: ATVALIDM Mask */

/* ITM Integration Read Register Definitions */
#define ITM_IRR_ATREADYM_Pos                0                                             /*!< ITM IRR: ATREADYM Position */
#define ITM_IRR_ATREADYM_Msk               (1UL /*<< ITM_IRR_ATREADYM_Pos*/)              /*!< ITM IRR: ATREADYM Mask */

/* ITM Integration Mode Control Register Definitions */
#define ITM_IMCR_INTEGRATION_Pos            0                                             /*!< ITM IMCR: INTEGRATION Position */
#define ITM_IMCR_INTEGRATION_Msk           (1UL /*<< ITM_IMCR_INTEGRATION_Pos*/)          /*!< ITM IMCR: INTEGRATION Mask */

/* ITM Lock Status Register Definitions */
#define ITM_LSR_ByteAcc_Pos                 2                                             /*!< ITM LSR: ByteAcc Position */
#define ITM_LSR_ByteAcc_Msk                (1UL << ITM_LSR_ByteAcc_Pos)                   /*!< ITM LSR: ByteAcc Mask */

#define ITM_LSR_Access_Pos                  1                                             /*!< ITM LSR: Access Position */
#define ITM_LSR_Access_Msk                 (1UL << ITM_LSR_Access_Pos)                    /*!< ITM LSR: Access Mask */

#define ITM_LSR_Present_Pos                 0                                             /*!< ITM LSR: Present Position */
#define ITM_LSR_Present_Msk                (1UL /*<< ITM_LSR_Present_Pos*/)               /*!< ITM LSR: Present Mask */

#endif
