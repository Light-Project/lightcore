/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_RAW_SUNIV_H_
#define _DEVICE_RAW_SUNIV_H_

#include <bits.h>

/*
 * KEYADC is 6-bit resolution ADC for key application.
 * The KEYADC can work up to 250Hz conversion rate.
 */

enum suniv_adc_registers {
    SUNIV_ADC_CTRL  = 0x00, // KEYADC Control Register
    SUNIV_ADC_INTC  = 0x04, // KEYADC Interrupt Control Register
    SUNIV_ADC_INTS  = 0x08, // KEYADC Interrupt Status Register
    SUNIV_ADC_DATA  = 0x0c, // KEYADC Data Register
};

/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */

#define SUNIV_ADC_CTRL_FIRST_DLY    (0xff<<24)  // First Conert Delay (n samples)
#define SUNIV_ADC_CTRL_CONT_TIME    (0x0f<<16)  // Continue Time Select (8*(n+1) sample)
#define SUNIV_ADC_CTRL_MODE         (0x03<<12)  // KEYADC Mode (0: Normal 1: Single 2: Continue)
#define SUNIV_ADC_CTRL_LEVELB_CNT   (0x0f<<8)   // Level A to B time (n+1)
#define SUNIV_ADC_CTRL_HOLD         BIT(6)      // KEYADC Sample hold Enable
#define SUNIV_ADC_CTRL_LEVELB       (0x03<<2)   // KEYADC Level B Value (val = 0x3c-(3*n))
#define SUNIV_ADC_CTRL_RATE         (0x03<<2)   // KEYADC Sample Rate (freq = 250/(2^n))
#define SUNIV_ADC_CTRL_EN           BIT(0)      // KEYADC Enable

#define SUNIV_ADC_INTC_KEYUP        BIT(4)      // KEYADC Key Up
#define SUNIV_ADC_INTC_ALRDY_HOLD   BIT(3)      // KEYADC Already Hold
#define SUNIV_ADC_INTC_HOLD         BIT(2)      // KEYADC Hold Key
#define SUNIV_ADC_INTC_KEYDOWN      BIT(1)      // KEYADC Key Down
#define SUNIV_ADC_INTC_DATA         BIT(0)      // KEYADC Data IRQ

#define SUNIV_ADC_DATA_MASK         (0x1f)      // KEYADC Data

#endif /* _DEVICE_RAW_SUNIV_H_ */
