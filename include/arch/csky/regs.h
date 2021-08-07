/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_CSKY_REGS_H_
#define _ARCH_CSKY_REGS_H_

#include <bits.h>

#define PSR_C       BIT(0)
#define PSR_AF      BIT(1)
#define PSR_FE      BIT(4)
#define PSR_IE      BIT(6)
#define PSR_IC      BIT(7)
#define PSR_EE      BIT(8)
#define PSR_MM      BIT(9)
#define PSR_TE      BIT(12)
#define PSR_TP      BIT(13)

#define CFR_SELI    BIT(0)
#define CFR_SELD    BIT(1)
#define CFR_INV     BIT(4)
#define CFR_CLR     BIT(5)
#define CFR_OMS     BIT(6)
#define CFR_ITS     BIT(7)
#define CFR_LICF    BIT(31)

#define CCR_VAILD   BIT(1)
#define CCR_DIRTY   BIT(2)
#define CCR_CACHE   BIT(3)


/* cp15 cr7 */

/* cp15 cr8 */
#define MCIR_ASID       BIT(0)
#define MCIR_TLBINVD    (0x03 << 24)
#define MCIR_TLBINVA    BIT(26)
#define MCIR_TLBINV     BIT(27)
#define MCIR_TLBWR      BIT(28)
#define MCIR_TLBWI      BIT(29)
#define MCIR_TLBR       BIT(30)
#define MCIR_TLBP       BIT(31)

/* cp15 cr29 */
#define MPGD_HRE        BIT(0)
#define MPGD_TLBINVD    (0x03 << 24)

/* cp15 cr30/31 */
#define MSA_VAILD   BIT(1)
#define MSA_DIRTY   BIT(2)
#define MSA_CACHE   BIT(3)

#endif  /* _ARCH_CSKY_REGS_H_ */
