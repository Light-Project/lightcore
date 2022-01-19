/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_I810_H_
#define _DRIVER_VIDEO_I810_H_

#include <bits.h>
#include <driver/video/vga.h>

enum i810_registers {
    /* Instruction and Interrupt Control Registers */
    I810_FENCE              = 0x02000,
    I810_PGTBL_CTL          = 0x02020,
    I810_PGTBL_ER           = 0x02024,
    I810_LRING              = 0x02030,
    I810_IRING              = 0x02040,
    I810_HWS_PGA            = 0x02080,
    I810_IPEIR              = 0x02088,
    I810_IPEHR              = 0x0208c,
    I810_INSTDONE           = 0x02090,
    I810_NOPID              = 0x02094,
    I810_HWSTAM             = 0x02098,
    I810_IER                = 0x020a0,
    I810_IIR                = 0x020a4,
    I810_IMR                = 0x020a8,
    I810_ISR                = 0x020ac,
    I810_EIR                = 0x020b0,
    I810_EMR                = 0x020b4,
    I810_ESR                = 0x020b8,
    I810_INSTPM             = 0x020c0,
    I810_INSTPS             = 0x020c4,
    I810_BBP_PTR            = 0x020c8,
    I810_ABB_SRT            = 0x020cc,
    I810_ABB_END            = 0x020d0,
    I810_DMA_FADD           = 0x020d4,
    I810_FW_BLC             = 0x020d8,
    I810_MEM_MODE           = 0x020dc,

    /* Display Cache Interface */
    I810_DRT                = 0x03000,  /* DRAM Row Type */
    I810_DRAMCL             = 0x03001,  /* DRAM Control Low */
    I810_DRAMCH             = 0x03002,  /* DRAM Control High */

    /* Span Cursor Registers */
    I810_UI_SC_CTL          = 0x04008,

    /* I/O Control Registers */
    I810_HVSYNC             = 0x05000,
    I810_GPIOA              = 0x05010,
    I810_GPIOB              = 0x05014,
    I810_GPIOC              = 0x0501c,

    /* Clock Control and Power Management Registers */
    I810_DCLK_0D            = 0x06000,
    I810_DCLK_1D            = 0x06004,
    I810_DCLK_2D            = 0x06008,
    I810_LCD_CLKD           = 0x0600c,
    I810_DCLK_0DS           = 0x06010,
    I810_PWR_CLKC           = 0x06014,

    /* Graphics Translation Table Range Definition */
    I810_GTT                = 0x10000,

    /* Overlay Registers */
    I810_OVOADDR            = 0x30000,
    I810_DOVOSTA            = 0x30008,
    I810_GAMMA              = 0x30010,
    I810_OBUF_0Y            = 0x30100,
    I810_OBUF_1Y            = 0x30104,
    I810_OBUF_0U            = 0x30108,
    I810_OBUF_0V            = 0x3010c,
    I810_OBUF_1U            = 0x30110,
    I810_OBUF_1V            = 0x30114,
    I810_OVOSTRIDE          = 0x30118,
    I810_YRGB_VPH           = 0x3011c,
    I810_UV_VPH             = 0x30120,
    I810_HORZ_PH            = 0x30124,
    I810_INIT_PH            = 0x30128,
    I810_DWINPOS            = 0x3012c,
    I810_DWINSZ             = 0x30130,
    I810_SWID               = 0x30134,
    I810_SWIDQW             = 0x30138,
    I810_SHEIGHT            = 0x3013f,
    I810_YRGBSCALE          = 0x30140,
    I810_UVSCALE            = 0x30144,
    I810_OVOCLRCO           = 0x30148,
    I810_OVOCLRC1           = 0x3014c,
    I810_DCLRKV             = 0x30150,
    I810_DLCRKM             = 0x30154,
    I810_SCLRKVH            = 0x30158,
    I810_SCLRKVL            = 0x3015c,
    I810_SCLRKM             = 0x30160,
    I810_OVOCONF            = 0x30164,
    I810_OVOCMD             = 0x30168,
    I810_AWINPOS            = 0x30170,
    I810_AWINZ              = 0x30174,

    /* BLT Engine Status (Software Debug) */
    I810_BR00               = 0x40000,
    I810_BRO1               = 0x40004,
    I810_BR02               = 0x40008,
    I810_BR03               = 0x4000c,
    I810_BR04               = 0x40010,
    I810_BR05               = 0x40014,
    I810_BR06               = 0x40018,
    I810_BR07               = 0x4001c,
    I810_BR08               = 0x40020,
    I810_BR09               = 0x40024,
    I810_BR10               = 0x40028,
    I810_BR11               = 0x4002c,
    I810_BR12               = 0x40030,
    I810_BR13               = 0x40034,
    I810_BR14               = 0x40038,
    I810_BR15               = 0x4003c,
    I810_BR16               = 0x40040,
    I810_BR17               = 0x40044,
    I810_BR18               = 0x40048,
    I810_BR19               = 0x4004c,
    I810_SSLADD             = 0x40074,
    I810_DSLH               = 0x40078,
    I810_DSLRADD            = 0x4007c,

    /* LCD/TV-Out and HW DVD Registers */
    I810_HTOTAL             = 0x60000,
    I810_HBLANK             = 0x60004,
    I810_HSYNC              = 0x60008,
    I810_VTOTAL             = 0x6000c,
    I810_VBLANK             = 0x60010,
    I810_VSYNC              = 0x60014,
    I810_LCDTV_C            = 0x60018,
    I810_OVRACT             = 0x6001c,
    I810_BCLRPAT            = 0x60020,

    /* Display and Cursor Control Registers */
    I810_DISP_SL            = 0x70000,
    I810_DISP_SLC           = 0x70004,
    I810_PIXCONF            = 0x70008,
    I810_PIXCONF1           = 0x70009,
    I810_BLTCNTL            = 0x7000c,
    I810_SWF                = 0x70014,
    I810_DPLYBASE           = 0x70020,
    I810_DPLYSTAS           = 0x70024,
    I810_CURCNTR            = 0x70080,
    I810_CURBASE            = 0x70084,
    I810_CURPOS             = 0x70088,

    /* SMRAM Registers */
    I810_SMRAM              = 0x10,

    /* Graphics Control Registers */
    I810_GR_INDEX           = 0x3ce,
    I810_GR_DATA            = 0x3cf,

    /* CRT Controller Registers */
    I810_CR_INDEX_MDA       = 0x3b4,
    I810_CR_INDEX_CGA       = 0x3d4,
    I810_CR_DATA_MDA        = 0x3b5,
    I810_CR_DATA_CGA        = 0x3d5,

    /* General Control and Status Registers */
    I810_ST00               = 0x3c2,
    I810_ST01_MDA           = 0x3ba,
    I810_ST01_CGA           = 0x3da,
    I810_FRC_READ           = 0x3ca,
    I810_FRC_WRITE_MDA      = 0x3ba,
    I810_FRC_WRITE_CGA      = 0x3da,
    I810_MSR_READ           = 0x3cc,
    I810_MSR_WRITE          = 0x3c2,

    /* Sequencer Registers */
    I810_SR_INDEX           = 0x3c4,
    I810_SR_DATA            = 0x3c5,

    /* Attribute Controller Registers */
    I810_ATTR_WRITE         = 0x3c0,
    I810_ATTR_READ          = 0x3c1,

    /* CLUT */
    I810_CLUT_DATA          = 0x3c9,
    I810_CLUT_INDEX_READ    = 0x3c7,
    I810_CLUT_INDEX_WRITE   = 0x3c8,
    I810_DACMASK            = 0x3c6,
};


enum i810_index {
    /* CRT Controller Registers */
    I810_CR_INDEX_CGA_00    = 0x00,
    I810_CR_INDEX_CGA_01    = 0x01,
    I810_CR_INDEX_CGA_02    = 0x02,
    I810_CR_INDEX_CGA_03    = 0x03,
    I810_CR_INDEX_CGA_04    = 0x04,
    I810_CR_INDEX_CGA_05    = 0x05,
    I810_CR_INDEX_CGA_06    = 0x06,
    I810_CR_INDEX_CGA_07    = 0x07,
    I810_CR_INDEX_CGA_08    = 0x08,
    I810_CR_INDEX_CGA_09    = 0x09,
    I810_CR_INDEX_CGA_0A    = 0x0A,
    I810_CR_INDEX_CGA_0B    = 0x0B,
    I810_CR_INDEX_CGA_0C    = 0x0C,
    I810_CR_INDEX_CGA_0D    = 0x0D,
    I810_CR_INDEX_CGA_0E    = 0x0E,
    I810_CR_INDEX_CGA_0F    = 0x0F,
    I810_CR_INDEX_CGA_10    = 0x10,
    I810_CR_INDEX_CGA_11    = 0x11,
    I810_CR_INDEX_CGA_12    = 0x12,
    I810_CR_INDEX_CGA_13    = 0x13,
    I810_CR_INDEX_CGA_14    = 0x14,
    I810_CR_INDEX_CGA_15    = 0x15,
    I810_CR_INDEX_CGA_16    = 0x16,
    I810_CR_INDEX_CGA_17    = 0x17,
    I810_CR_INDEX_CGA_18    = 0x18,
    I810_CR_INDEX_CGA_30    = 0x30,
    I810_CR_INDEX_CGA_31    = 0x31,
    I810_CR_INDEX_CGA_32    = 0x32,
    I810_CR_INDEX_CGA_33    = 0x33,
    I810_CR_INDEX_CGA_35    = 0x35,
    I810_CR_INDEX_CGA_39    = 0x39,
    I810_CR_INDEX_CGA_40    = 0x40,
    I810_CR_INDEX_CGA_41    = 0x41,
    I810_CR_INDEX_CGA_42    = 0x42,
    I810_CR_INDEX_CGA_70    = 0x70,
    I810_CR_INDEX_CGA_80    = 0x80,
    I810_CR_INDEX_CGA_81    = 0x82,

    I810_SR_INDEX_01        = 0x01,
    I810_SR_INDEX_02        = 0x02,
    I810_SR_INDEX_03        = 0x03,
    I810_SR_INDEX_04        = 0x04,
    I810_SR_INDEX_07        = 0x07,

    /* Graphics Controller Registers */
    I810_GR00               = 0x00,
    I810_GR01               = 0x01,
    I810_GR02               = 0x02,
    I810_GR03               = 0x03,
    I810_GR04               = 0x04,
    I810_GR05               = 0x05,
    I810_GR06               = 0x06,
    I810_GR07               = 0x07,
    I810_GR08               = 0x08,
    I810_GR10               = 0x10,
    I810_GR11               = 0x11,
};

/************************************************************************************************/
/*      Mnemonic                    value              meaning/usage                            */

#define I810_SR01_SCR               BIT(5)
#define I810_DRAMCH_DRAM            BIT(3)

#define I810_IRING_START            0xfffff000
#define I810_IRING_SIZE             0x001ff000
#define I810_IRING_HEAD             0x001ffffc
#define I810_IRING_TAIL             0x001ffff8

#define CURSOR_ENABLE_MASK          0x1000
#define CURSOR_MODE_64_TRANS        4
#define CURSOR_MODE_64_XOR          5
#define CURSOR_MODE_64_3C           6
#define COORD_INACTIVE              0
#define COORD_ACTIVE                (1 << 4)
#define EXTENDED_PALETTE            1

#endif /* _DRIVER_VIDEO_I810_H_ */
