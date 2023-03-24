/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PLATFORM_IBMPC_H_
#define _DRIVER_PLATFORM_IBMPC_H_

#include <bits.h>

#define IBMPC_NMISC_BASE    0x61
#define IBMPC_FA20_BASE     0x92
#define IBMPC_QA20_BASE     0xee

/****************************************************************************************/
/*      Mnemonic                        value              meaning/usage                */

/* NMI Status And Control bitflag definitions */
#define IBMPC_NMISC_NMI_SERR_STAT       BIT(7)          /* SERR# NMI Source Status */
#define IBMPC_NMISC_NMI_IOCHK_STAT      BIT(6)          /* IOCHK# NMI Source Status */
#define IBMPC_NMISC_SPEAKER_STAT        BIT(5)          /* Timer Counter 2 OUT Status */
#define IBMPC_NMISC_REFRESH_TOGGLE      BIT(4)          /* Refresh Cycle Toggle */
#define IBMPC_NMISC_NMI_IOCHK           BIT(3)          /* IOCHK# NMI Enable */
#define IBMPC_NMISC_PCI_SERR            BIT(2)          /* PCI SERR# Enable */
#define IBMPC_NMISC_SPEAKER_EN          BIT(1)          /* Speaker Data Enable */
#define IBMPC_NMISC_TIMER2_EN           BIT(0)          /* Timer Counter 2 Enable */

/* Fast A20 and Init bitflag definitions */
#define IBMPC_FA20_GATE                 BIT(1)          /* Alternate A20 Gate */
#define IBMPC_FA20_INIT                 BIT(0)          /* Init Now */

#endif /* _DRIVER_PLATFORM_IBMPC_H_ */
