/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_WATCHDOG_TCO_
#define _DRIVER_WATCHDOG_TCO_

#include <bits.h>

enum tco_registers {
    TCO_RLD         = 0x00, /* TCO Timer Reload/Curr. Value */
    TCO_TMRV1       = 0x12, /* TCOv1 Timer Initial Value */
    TCO_DAT_IN      = 0x02, /* TCO Data In Register */
    TCO_DAT_OUT     = 0x03, /* TCO Data Out Register */
    TCO_STS1        = 0x04, /* TCO1 Status Register */
    TCO_STS2        = 0x06, /* TCO2 Status Register */
    TCO_CNT1        = 0x08, /* TCO1 Control Register */
    TCO_CNT2        = 0x0a, /* TCO2 Control Register */
    TCO_MESSAGE     = 0x0c, /*  */
    TCO_WDTSTATUS   = 0x0e, /*  */
    TCO_LEGACY_ELIM = 0x10, /*  */
    TCO_TMRV2       = 0x12, /* TCOv2 Timer Initial Value */
};

#define TCO_RLD_COUNT   BIT_RANGE(9, 0)

#define TCO_MATCH_ID(ver)  "intel,tco" __stringify(ver)

#endif  /* _DRIVER_WATCHDOG_TCO_ */
