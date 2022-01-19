/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_AMD_K10_H_
#define _DRIVER_HWMON_AMD_K10_H_

#define DELL_SMM_FN_STATUS      0x0025
#define DELL_SMM_POWER_STATUS   0x0069
#define DELL_SMM_SET_FAN        0x01a3
#define DELL_SMM_GET_FAN        0x00a3
#define DELL_SMM_GET_SPEED      0x02a3
#define DELL_SMM_GET_FAN_TYPE   0x03a3
#define DELL_SMM_GET_NOM_SPEED  0x04a3
#define DELL_SMM_GET_TEMP       0x10a3
#define DELL_SMM_GET_TEMP_TYPE  0x11a3
#define DELL_SMM_GET_SIG1       0xfea3
#define DELL_SMM_GET_SIG2       0xffa3

#define DELL_FAN_MULT           30
#define DELL_FAN_MAX_RPM        30000
#define DELL_MAX_TEMP           127

#define DELL_FN_NONE            0x00
#define DELL_FN_UP              0x01
#define DELL_FN_DOWN            0x02
#define DELL_FN_MUTE            0x04
#define DELL_FN_MASK            0x07
#define DELL_FN_SHIFT           8

#define DELL_POWER_AC           0x05
#define DELL_POWER_BATTERY      0x01

#define DELL_SMM_NO_TEMP        10
#define DELL_SMM_NO_FANS        3

#define DELL_FAN_LEFT           1
#define DELL_FAN_RIGHT          0
#define DELL_FAN_OFF            0
#define DELL_FAN_LOW            1
#define DELL_FAN_HIGH           2
#define DELL_FAN_TURBO          3
#define DELL_FAN_MAX            DELL_FAN_TURBO

#endif  /* _DRIVER_HWMON_AMD_K10_H_ */
