/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_POWER_PIIX_H_
#define _DRIVER_POWER_PIIX_H_

#include <bits.h>

#define PIIX_RC_FULL    BIT(3)  /* Full Power Reset */
#define PIIX_RC_RCPU    BIT(2)  /* CPU Soft Restart */
#define PIIX_RC_SRST    BIT(1)  /* System Hard Reset */

#endif /* _DRIVER_POWER_PIIX_H_ */
