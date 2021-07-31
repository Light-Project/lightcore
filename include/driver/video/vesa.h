/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _DRIVER_GPU_VESA_H_
#define _DRIVER_GPU_VESA_H_

#include <bits.h>

enum VESA_Registers {
    /* VGA data register ports */
    VESA_CRT_DC     = 0x3D5,    /* CRT Controller Data Register - color emulation */
    VESA_CRT_DM     = 0x3B5,    /* CRT Controller Data Register - mono emulation */
    VESA_ATT_R      = 0x3C1,    /* Attribute Controller Data Read Register */
    VESA_ATT_W      = 0x3C0,    /* Attribute Controller Data Write Register */
    VESA_GFX_D      = 0x3CF,    /* Graphics Controller Data Register */
    VESA_SEQ_D      = 0x3C5,    /* Sequencer Data Register */
    VESA_MIS_R      = 0x3CC,    /* Misc Output Read Register */
    VESA_MIS_W      = 0x3C2,    /* Misc Output Write Register */
    VESA_FTC_R      = 0x3CA,    /* Feature Control Read Register */
    VESA_IS1_RC     = 0x3DA,    /* Input Status Register 1 - color emulation */
    VESA_IS1_RM     = 0x3BA,    /* Input Status Register 1 - mono emulation */
    VESA_PEL_D      = 0x3C9,    /* PEL Data Register */
    VESA_PEL_MSK    = 0x3C6,    /* PEL mask register */

    /* EGA-specific registers */
    VESAE_GFX_E0    = 0x3CC,    /* Graphics enable processor 0 */
    VESAE_GFX_E1    = 0x3CA,    /* Graphics enable processor 1 */

    /* VGA index register ports */
    VESA_CRT_IC     = 0x3D4,    /* CRT Controller Index - color emulation */
    VESA_CRT_IM     = 0x3B4,    /* CRT Controller Index - mono emulation */
    VESA_ATT_IW     = 0x3C0,    /* Attribute Controller Index & Data Write Register */
    VESA_GFX_I      = 0x3CE,    /* Graphics Controller Index */
    VESA_SEQ_I      = 0x3C4,    /* Sequencer Index */
    VESA_PEL_IW     = 0x3C8,    /* PEL Write Index */
    VESA_PEL_IR     = 0x3C7,    /* PEL Read Index */
};

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

/* VGA misc register bit masks */
#define VESA_MIS_COLOR                  BIT(0)
#define VESA_MIS_ENB_MEM_ACCESS         BIT(1)
#define VESA_MIS_DCLK_28322_720         BIT(2)
#define VESA_MIS_ENB_PLL_LOAD           BIT(2) | BIT(3)
#define VESA_MIS_SEL_HIGH_PAGE          BIT(5)

/* VGA CRT controller register indices */
#define VESA_CRTC_H_TOTAL               0x00
#define VESA_CRTC_H_DISP                0x01
#define VESA_CRTC_H_BLANK_START         0x02
#define VESA_CRTC_H_BLANK_END           0x03
#define VESA_CRTC_H_SYNC_START          0x04
#define VESA_CRTC_H_SYNC_END            0x05
#define VESA_CRTC_V_TOTAL               0x06
#define VESA_CRTC_OVERFLOW              0x07
#define VESA_CRTC_PRESET_ROW            0x08
#define VESA_CRTC_MAX_SCAN              0x09
#define VESA_CRTC_CURSOR_START          0x0A
#define VESA_CRTC_CURSOR_END            0x0B
#define VESA_CRTC_START_HI              0x0C
#define VESA_CRTC_START_LO              0x0D
#define VESA_CRTC_CURSOR_HI             0x0E
#define VESA_CRTC_CURSOR_LO             0x0F
#define VESA_CRTC_V_SYNC_START          0x10
#define VESA_CRTC_V_SYNC_END            0x11
#define VESA_CRTC_V_DISP_END            0x12
#define VESA_CRTC_OFFSET                0x13
#define VESA_CRTC_UNDERLINE             0x14
#define VESA_CRTC_V_BLANK_START         0x15
#define VESA_CRTC_V_BLANK_END           0x16
#define VESA_CRTC_MODE                  0x17
#define VESA_CRTC_LINE_COMPARE          0x18

/* VGA CRT controller bit masks */
#define VESA_CR11_LOCK_CR0_CR7          0x80    /* lock writes to CR0 - CR7 */
#define VESA_CR17_H_V_SIGNALS_ENABLED   0x80

/* VGA attribute controller register indices */
#define VESA_ATC_PALETTE0               0x00
#define VESA_ATC_PALETTE1               0x01
#define VESA_ATC_PALETTE2               0x02
#define VESA_ATC_PALETTE3               0x03
#define VESA_ATC_PALETTE4               0x04
#define VESA_ATC_PALETTE5               0x05
#define VESA_ATC_PALETTE6               0x06
#define VESA_ATC_PALETTE7               0x07
#define VESA_ATC_PALETTE8               0x08
#define VESA_ATC_PALETTE9               0x09
#define VESA_ATC_PALETTEA               0x0A
#define VESA_ATC_PALETTEB               0x0B
#define VESA_ATC_PALETTEC               0x0C
#define VESA_ATC_PALETTED               0x0D
#define VESA_ATC_PALETTEE               0x0E
#define VESA_ATC_PALETTEF               0x0F
#define VESA_ATC_MODE                   0x10
#define VESA_ATC_OVERSCAN               0x11
#define VESA_ATC_PLANE_ENABLE           0x12
#define VESA_ATC_PEL                    0x13
#define VESA_ATC_COLOR_PAGE             0x14

#define VESA_AR_ENABLE_DISPLAY          0x20

/* VGA sequencer register indices */
#define VESA_SEQ_RESET                  0x00
#define VESA_SEQ_CLOCK_MODE             0x01
#define VESA_SEQ_PLANE_WRITE            0x02
#define VESA_SEQ_CHARACTER_MAP          0x03
#define VESA_SEQ_MEMORY_MODE            0x04

/* VGA sequencer register bit masks */
#define VESA_SR01_CHAR_CLK_8DOTS        0x01    /* bit 0: character clocks 8 dots wide are generated */
#define VESA_SR01_SCREEN_OFF            0x20    /* bit 5: Screen is off */
#define VESA_SR02_ALL_PLANES            0x0F    /* bits 3-0: enable access to all planes */
#define VESA_SR04_EXT_MEM               0x02    /* bit 1: allows complete mem access to 256K */
#define VESA_SR04_SEQ_MODE              0x04    /* bit 2: directs system to use a sequential addressing mode */
#define VESA_SR04_CHN_4M                0x08    /* bit 3: selects modulo 4 addressing for CPU access to display memory */

/* VGA graphics controller register indices */
#define VESA_GFX_SR_VALUE               0x00
#define VESA_GFX_SR_ENABLE              0x01
#define VESA_GFX_COMPARE_VALUE          0x02
#define VESA_GFX_DATA_ROTATE            0x03
#define VESA_GFX_PLANE_READ             0x04
#define VESA_GFX_MODE                   0x05
#define VESA_GFX_MISC                   0x06
#define VESA_GFX_COMPARE_MASK           0x07
#define VESA_GFX_BIT_MASK               0x08

/* VGA graphics controller bit masks */
#define VESA_GR06_GRAPHICS_MODE         0x01

#endif  /* _DRIVER_GPU_VESA_H_ */
