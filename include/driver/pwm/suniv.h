/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_PWM_SUNIV_H_
#define _DRIVER_PWM_SUNIV_H_

#include <bits.h>

enum suniv_pwm_registers {
    SUNIV_PWM_CTRL  = 0x0,
    SUNIV_PWM_CH0   = 0x4,
    SUNIV_PWM_CH1   = 0x8,
};

/************************************************************************************************/
/*      Mnemonic                    value                meaning/usage                          */

#define SUNIV_PWM_CTRL_CH1_RDY      BIT(29)             /* PWM1 period register ready */
#define SUNIV_PWM_CTRL_CH0_RDY      BIT(28)             /* PWM0 period register ready */
#define SUNIV_PWM_CTRL_CH1_BYPASS   BIT(24)             /* PWM Channel 1 bypass enable (output 24MHz) */
#define SUNIV_PWM_CTRL_CH1_PULSE    BIT(23)             /* PWM Channel 1 pulse output start */
#define SUNIV_PWM_CTRL_CH1_MODE     BIT(22)             /* PWM Channel 1 mode (0: Cycle 1: Pulse) */
#define SUNIV_PWM_CTRL_CH1_GATE     BIT(21)             /* PWM Channel 1 Clock Gating */
#define SUNIV_PWM_CTRL_CH1_STA      BIT(20)             /* PWM Channel 1 Active State */
#define SUNIV_PWM_CTRL_CH1_EN       BIT(19)             /* PWM Channel 1 Enable */
#define SUNIV_PWM_CTRL_CH1_PRESCAL  BIT_RANGE(18, 15)   /* PWM Channel 1 Pre-scalar */
#define SUNIV_PWM_CTRL_CH0_BYPASS   BIT(9)              /* PWM Channel 0 bypass enable (output 24MHz) */
#define SUNIV_PWM_CTRL_CH0_PULSE    BIT(8)              /* PWM Channel 0 pulse output start */
#define SUNIV_PWM_CTRL_CH0_MODE     BIT(7)              /* PWM Channel 0 mode (0: Cycle 1: Pulse) */
#define SUNIV_PWM_CTRL_CH0_GATE     BIT(6)              /* PWM Channel 0 Clock Gating */
#define SUNIV_PWM_CTRL_CH0_STA      BIT(5)              /* PWM Channel 0 Active State */
#define SUNIV_PWM_CTRL_CH0_EN       BIT(4)              /* PWM Channel 0 Enable */
#define SUNIV_PWM_CTRL_CH0_PRESCAL  BIT_RANGE(3, 0)     /* PWM Channel 0 Pre-scalar */

#define SUNIV_PWM_CH0_ENTIRE_CYS    BIT_RANGE(31, 16)   /* Number of the entire cycles (24M/n) */
#define SUNIV_PWM_CH0_ACT_CYS       BIT_RANGE(15, 0)    /* Number of the active cycles (24M/n) */

#define SUNIV_PWM_CH1_ENTIRE_CYS    BIT_RANGE(31, 16)   /* Number of the entire cycles (24M/n) */
#define SUNIV_PWM_CH1_ACT_CYS       BIT_RANGE(15, 0)    /* Number of the active cycles (24M/n) */

#endif  /* _DRIVER_PWM_SUNIV_H_ */
