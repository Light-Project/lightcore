#ifndef _ARCH_ARME_NVIC_H_
#define _ARCH_ARME_NVIC_H_

#include <types.h>
#include <arch/arme/map.h>

typedef struct
{
    uint32_t ISER[8];                 /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           */
    uint32_t RESERVED0[24];
    uint32_t ICER[8];                 /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register         */
    uint32_t RSERVED1[24];
    uint32_t ISPR[8];                 /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register          */
    uint32_t RESERVED2[24];
    uint32_t ICPR[8];                 /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register        */
    uint32_t RESERVED3[24];
    uint32_t IABR[8];                 /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register           */
    uint32_t RESERVED4[56];
    uint8_t  IP[240];                 /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
    uint32_t RESERVED5[644];
    uint32_t STIR;                    /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register     */
}  NVIC_Type;

#define NVIC    ((NVIC_Type *)NVIC_BASE)

/* Software Triggered Interrupt Register Definitions */
#define NVIC_STIR_INTID_Pos                 0                                          /*!< STIR: INTLINESNUM Position */
#define NVIC_STIR_INTID_Msk                (0x1FFUL /*<< NVIC_STIR_INTID_Pos*/)        /*!< STIR: INTLINESNUM Mask */

#endif /* _ARCH_ARME_NVIC_H_ */
