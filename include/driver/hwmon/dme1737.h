/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_HWMON_DME1737_H_
#define _DRIVER_HWMON_DME1737_H_

#include <bits.h>

enum dme1737_register {
    DME1737_INDEX_ADDR      = 0x00,
    DME1737_INDEX_DATA      = 0x01,
};

enum dme1737_sio_index {
    DME1737_SIO_ENTRY       = 0x55,
    DME1737_SIO_EXIT        = 0xaa,

    DME1737_SIO_CCTL        = 0x02,
    DME1737_SIO_LOGICAL     = 0x07,
    DME1737_SIO_DEVID       = 0x20,
    DME1737_SIO_DEVREV      = 0x21,
    DME1737_SIO_POWER       = 0x22,
    DME1737_SIO_BASE_MSB    = 0x60,
    DME1737_SIO_BASE_LSB    = 0x61,
};

enum dme1737_runtime_registers {
    DME1737_RTM_LED0        = 0x5d,
    DME1737_RTM_LED1        = 0x5e,
    DME1737_RTM_HWM_ADDR    = 0x70,
    DME1737_RTM_HWM_DATA    = 0x71,
};

enum dme1737_hwmon_index {
    DME1737_HWM_VIN0        = 0x20,
    DME1737_HWM_TEMP        = 0x25,
    DME1737_HWM_FAN         = 0x28,
    DME1737_HWM_DEVICE      = 0x3d,
    DME1737_HWM_COMPANY     = 0x3e,
    DME1737_HWM_VERSTEP     = 0x3f,
    DME1737_HWM_CONFIG      = 0x40,
    DME1737_HWM_VID         = 0x43,
    DME1737_HWM_VLMIN0      = 0x44,
    DME1737_HWM_VLMAX0      = 0x45,
    DME1737_HWM_TLMIN       = 0x4e,
    DME1737_HWM_TLMAX       = 0x4f,
    DME1737_HWM_CONFIG1     = 0x7f,
    DME1737_HWM_TACH_PWM    = 0x81,
    DME1737_HWM_ADLSB       = 0x84,
    DME1737_HWM_VIN1        = 0x99,
    DME1737_HWM_VLMIN1      = 0x9b,
    DME1737_HWM_VLMAX1      = 0x9c,
};

enum dme1737_devid {
    SCH3112_ID  = 0x7c,
    SCH3114_ID  = 0x7d,
    SCH3116_ID  = 0x7f,
    SCH5127_ID  = 0x86,
    SCH5147_ID  = 0xc1,
};

#define DME1737_SIO_BASE0 0x002e
#define DME1737_SIO_BASE1 0x004e
#define DME1737_SIO_BASE2 0x162e
#define DME1737_SIO_BASE3 0x164e

#define DME1737_ADDR(base) ((base) + DME1737_INDEX_ADDR)
#define DME1737_DATA(base) ((base) + DME1737_INDEX_DATA)

#define DME1737_HWM_VIN(idx) (((idx) < 5 ? DME1737_HWM_VIN0 : DME1737_HWM_VIN1 - 5) + (idx))
#define DME1737_HWM_VMIN(idx) (((idx) < 5 ? DME1737_HWM_VLMIN0 : DME1737_HWM_VLMIN1 - 10) + (idx) * 2)
#define DME1737_HWM_VMAX(idx) (((idx) < 5 ? DME1737_HWM_VLMAX0 : DME1737_HWM_VLMAX1 - 10) + (idx) * 2)

#define DME1737_HWM_TEMP(idx) (DME1737_HWM_TEMP + (idx))
#define DME1737_HWM_TMIN(idx) (DME1737_HWM_TLMIN + (idx) * 2)
#define DME1737_HWM_TMAX(idx) (DME1737_HWM_TLMAX + (idx) * 2)
#define DME1737_HWM_ADLSB(idx) (DME1737_HWM_ADLSB + (idx))

#define DME1737_HWM_CONFIG_VBAT_MON     BIT(4)
#define DME1737_HWM_CONFIG_OVRID        BIT(3)
#define DME1737_HWM_CONFIG_READY        BIT(2)
#define DME1737_HWM_CONFIG_LOCK         BIT(1)
#define DME1737_HWM_CONFIG_START        BIT(0)

#endif  /* _DRIVER_HWMON_DME1737_H_ */
