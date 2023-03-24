/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SOUND_SUNIV_H_
#define _DRIVER_SOUND_SUNIV_H_

enum suniv_sound_registers {
    SUNIV_AC_DAC_DPC            = 0x00,
    SUNIV_AC_DAC_FIFOC          = 0x04,
    SUNIV_AC_DAC_FIFOS          = 0x08,
    SUNIV_AC_DAC_TXDATA         = 0x0c,
    SUNIV_AC_ADC_FIFOC          = 0x10,
    SUNIV_AC_ADC_FIFOS          = 0x14,
    SUNIV_AC_ADC_RXDATA         = 0x18,

    SUNIV_DAC_MIXER_CTRL        = 0x20,
    SUNIV_ADC_MIXER_CTRL        = 0x24,
    SUNIV_ADDA_TUNE             = 0x28,
    SUNIV_BIAS_DA16_CAL_CTRL0   = 0x2c,
    SUNIV_BIAS_DA16_CAL_CTRL1   = 0x34,

    SUNIV_AC_DAC_CNT            = 0x40,
    SUNIV_AC_ADC_CNT            = 0x44,
    SUNIV_AC_DAC_DG             = 0x48,
    SUNIV_AC_ADC_DG             = 0x4c,
    SUNIV_AC_ADC_DAP_CTR        = 0x70,
    SUNIV_AC_ADC_DAP_LCTR       = 0x74,
    SUNIV_AC_ADC_DAP_RCTR       = 0x78,
    SUNIV_AC_ADC_DAP_PARA       = 0x7c,
    SUNIV_AC_ADC_DAP_LAC        = 0x80,
    SUNIV_AC_ADC_DAP_LDAT       = 0x84,
    SUNIV_AC_ADC_DAP_RAC        = 0x88,
    SUNIV_AC_ADC_DAP_RDAT       = 0x8c,

    SUNIV_ADC_DAP_HPFC          = 0x90,
    SUNIV_ADC_DAP_LINAC         = 0x94,
    SUNIV_ADC_DAP_RINAC         = 0x98,
    SUNIV_ADC_DAP_ORT           = 0x9c,
};


#endif /* _DRIVER_SOUND_SUNIV_H_ */
