/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_AMD_K10_H_
#define _DRIVER_HWMON_AMD_K10_H_

/* miscellaneous (PCI function 3) */
#define K10_HARDWARE_THERMAL_CONTROL    0x64
#define K10_HARDWARE_THERMAL_HTC_ENABLE BIT(0)
#define K10_REPORTED_TEMPERATURE        0xa4

#define K10_TEMP_MAX    (70 * 1000)

#endif /* _DRIVER_HWMON_AMD_K10_H_ */
