#ifndef _ARCH_ARME_SCNSCB_H_
#define _ARCH_ARME_SCNSCB_H_

#include <types.h>
#include <arch/arme/map.h>

typedef struct
{
    uint32_t RESERVED0[1];
    uint32_t ICTR;                    /*!< Offset: 0x004 (R/ )  Interrupt Controller Type Register      */
    uint32_t ACTLR;                   /*!< Offset: 0x008 (R/W)  Auxiliary Control Register              */
} SCnSCB_Type;

#define SCnSCB      ((SCnSCB_Type *)SCS_BASE)

/* Interrupt Controller Type Register Definitions */
#define SCnSCB_ICTR_INTLINESNUM_Pos         0                                          /*!< ICTR: INTLINESNUM Position */
#define SCnSCB_ICTR_INTLINESNUM_Msk        (0xFUL /*<< SCnSCB_ICTR_INTLINESNUM_Pos*/)  /*!< ICTR: INTLINESNUM Mask */

/* Auxiliary Control Register Definitions */
#define SCnSCB_ACTLR_DISITMATBFLUSH_Pos    12                                          /*!< ACTLR: DISITMATBFLUSH Position */
#define SCnSCB_ACTLR_DISITMATBFLUSH_Msk    (1UL << SCnSCB_ACTLR_DISITMATBFLUSH_Pos)    /*!< ACTLR: DISITMATBFLUSH Mask */

#define SCnSCB_ACTLR_DISRAMODE_Pos         11                                          /*!< ACTLR: DISRAMODE Position */
#define SCnSCB_ACTLR_DISRAMODE_Msk         (1UL << SCnSCB_ACTLR_DISRAMODE_Pos)         /*!< ACTLR: DISRAMODE Mask */

#define SCnSCB_ACTLR_FPEXCODIS_Pos         10                                          /*!< ACTLR: FPEXCODIS Position */
#define SCnSCB_ACTLR_FPEXCODIS_Msk         (1UL << SCnSCB_ACTLR_FPEXCODIS_Pos)         /*!< ACTLR: FPEXCODIS Mask */

#define SCnSCB_ACTLR_DISFOLD_Pos            2                                          /*!< ACTLR: DISFOLD Position */
#define SCnSCB_ACTLR_DISFOLD_Msk           (1UL << SCnSCB_ACTLR_DISFOLD_Pos)           /*!< ACTLR: DISFOLD Mask */

#define SCnSCB_ACTLR_DISMCYCINT_Pos         0                                          /*!< ACTLR: DISMCYCINT Position */
#define SCnSCB_ACTLR_DISMCYCINT_Msk        (1UL /*<< SCnSCB_ACTLR_DISMCYCINT_Pos*/)    /*!< ACTLR: DISMCYCINT Mask */

#endif /* _ARCH_ARME_SCNSCB_H_ */
