/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_VGA_H_
#define _DRIVER_VIDEO_VGA_H_

#include <bits.h>

enum VGA_Registers {
    /* VGA data register ports */
    VGA_CRT_DC      = 0x3d5, /* CRT Controller Data Register - color emulation */
    VGA_CRT_DM      = 0x3b5, /* CRT Controller Data Register - mono emulation */
    VGA_ATT_R       = 0x3c1, /* Attribute Controller Data Read Register */
    VGA_ATT_W       = 0x3c0, /* Attribute Controller Data Write Register */
    VGA_GFX_D       = 0x3cf, /* Graphics Controller Data Register */
    VGA_SEQ_D       = 0x3c5, /* Sequencer Data Register */
    VGA_MIS_R       = 0x3cc, /* Misc Output Read Register */
    VGA_MIS_W       = 0x3c2, /* Misc Output Write Register */
    VGA_FTC_R       = 0x3ca, /* Feature Control Read Register */
    VGA_IS1_RC      = 0x3da, /* Input Status Register 1 - color emulation */
    VGA_IS1_RM      = 0x3ba, /* Input Status Register 1 - mono emulation */
    VGA_PEL_D       = 0x3c9, /* PEL Data Register */
    VGA_PEL_MSK     = 0x3c6, /* PEL mask register */

    /* EGA-specific registers */
    EGA_GFX_E0      = 0x3cc, /* Graphics enable processor 0 */
    EGA_GFX_E1      = 0x3ca, /* Graphics enable processor 1 */

    /* VGA index register ports */
    VGA_CRT_IC      = 0x3d4, /* CRT Controller Index - color emulation */
    VGA_CRT_IM      = 0x3b4, /* CRT Controller Index - mono emulation */
    VGA_ATT_IW      = 0x3c0, /* Attribute Controller Index & Data Write Register */
    VGA_GFX_I       = 0x3ce, /* Graphics Controller Index */
    VGA_SEQ_I       = 0x3c4, /* Sequencer Index */
    VGA_PEL_IW      = 0x3c8, /* PEL Write Index */
    VGA_PEL_IR      = 0x3c7, /* PEL Read Index */
};

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

/* VGA misc register bit masks */
#define VGA_MIS_COLOR                  BIT(0)
#define VGA_MIS_ENB_MEM_ACCESS         BIT(1)
#define VGA_MIS_DCLK_28322_720         BIT(2)
#define VGA_MIS_ENB_PLL_LOAD           BIT(2) | BIT(3)
#define VGA_MIS_SEL_HIGH_PAGE          BIT(5)

/* VGA CRT controller register indices */
#define VGA_CRTC_H_TOTAL               0x00
#define VGA_CRTC_H_DISP                0x01
#define VGA_CRTC_H_BLANK_START         0x02
#define VGA_CRTC_H_BLANK_END           0x03
#define VGA_CRTC_H_SYNC_START          0x04
#define VGA_CRTC_H_SYNC_END            0x05
#define VGA_CRTC_V_TOTAL               0x06
#define VGA_CRTC_OVERFLOW              0x07
#define VGA_CRTC_PRESET_ROW            0x08
#define VGA_CRTC_MAX_SCAN              0x09
#define VGA_CRTC_CURSOR_START          0x0A
#define VGA_CRTC_CURSOR_END            0x0B
#define VGA_CRTC_START_HI              0x0C
#define VGA_CRTC_START_LO              0x0D
#define VGA_CRTC_CURSOR_HI             0x0E
#define VGA_CRTC_CURSOR_LO             0x0F
#define VGA_CRTC_V_SYNC_START          0x10
#define VGA_CRTC_V_SYNC_END            0x11
#define VGA_CRTC_V_DISP_END            0x12
#define VGA_CRTC_OFFSET                0x13
#define VGA_CRTC_UNDERLINE             0x14
#define VGA_CRTC_V_BLANK_START         0x15
#define VGA_CRTC_V_BLANK_END           0x16
#define VGA_CRTC_MODE                  0x17
#define VGA_CRTC_LINE_COMPARE          0x18

/* VGA CRT controller bit masks */
#define VGA_CR11_LOCK_CR0_CR7          0x80
#define VGA_CR17_H_V_SIGNALS_ENABLED   0x80

/* VGA attribute controller register indices */
#define VGA_ATC_PALETTE0               0x00
#define VGA_ATC_PALETTE1               0x01
#define VGA_ATC_PALETTE2               0x02
#define VGA_ATC_PALETTE3               0x03
#define VGA_ATC_PALETTE4               0x04
#define VGA_ATC_PALETTE5               0x05
#define VGA_ATC_PALETTE6               0x06
#define VGA_ATC_PALETTE7               0x07
#define VGA_ATC_PALETTE8               0x08
#define VGA_ATC_PALETTE9               0x09
#define VGA_ATC_PALETTEA               0x0A
#define VGA_ATC_PALETTEB               0x0B
#define VGA_ATC_PALETTEC               0x0C
#define VGA_ATC_PALETTED               0x0D
#define VGA_ATC_PALETTEE               0x0E
#define VGA_ATC_PALETTEF               0x0F
#define VGA_ATC_MODE                   0x10
#define VGA_ATC_OVERSCAN               0x11
#define VGA_ATC_PLANE_ENABLE           0x12
#define VGA_ATC_PEL                    0x13
#define VGA_ATC_COLOR_PAGE             0x14

#define VGA_AR_ENABLE_DISPLAY          0x20

/* VGA sequencer register indices */
#define VGA_SEQ_RESET                  0x00
#define VGA_SEQ_CLOCK_MODE             0x01
#define VGA_SEQ_PLANE_WRITE            0x02
#define VGA_SEQ_CHARACTER_MAP          0x03
#define VGA_SEQ_MEMORY_MODE            0x04

/* VGA sequencer register bit masks */
#define VGA_SR01_CHAR_CLK_8DOTS        0x01    /* bit 0: character clocks 8 dots wide are generated */
#define VGA_SR01_SCREEN_OFF            0x20    /* bit 5: Screen is off */
#define VGA_SR02_ALL_PLANES            0x0F    /* bits 3-0: enable access to all planes */
#define VGA_SR04_EXT_MEM               0x02    /* bit 1: allows complete mem access to 256K */
#define VGA_SR04_SEQ_MODE              0x04    /* bit 2: directs system to use a sequential addressing mode */
#define VGA_SR04_CHN_4M                0x08    /* bit 3: selects modulo 4 addressing for CPU access to display memory */

/* VGA graphics controller register indices */
#define VGA_GFX_SR_VALUE               0x00
#define VGA_GFX_SR_ENABLE              0x01
#define VGA_GFX_COMPARE_VALUE          0x02
#define VGA_GFX_DATA_ROTATE            0x03
#define VGA_GFX_PLANE_READ             0x04
#define VGA_GFX_MODE                   0x05
#define VGA_GFX_MISC                   0x06
#define VGA_GFX_COMPARE_MASK           0x07
#define VGA_GFX_BIT_MASK               0x08

/* VGA graphics controller bit masks */
#define VGA_GR06_GRAPHICS_MODE         0x01

#endif /* _DRIVER_VIDEO_VGA_H_ */
