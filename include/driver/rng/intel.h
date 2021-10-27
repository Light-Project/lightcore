/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_RNG_INTEL_H_
#define _DRIVER_RNG_INTEL_H_

#include <bits.h>

enum intel_rng_registers {
    INTEL_RNG_HW        = 0x00,
    INTEL_RNG_STATUS    = 0x01,
    INTEL_RNG_DATA      = 0x02,
};

#define INTEL_RNG_HW_ENABLE     BIT(0)
#define INTEL_RNG_HW_PRESENT    BIT(6)

#define INTEL_RNG_BASE  0xffbc015f
#define INTEL_RNG_SIZE  0x03

#endif  /* _DRIVER_RNG_INTEL_H_ */
