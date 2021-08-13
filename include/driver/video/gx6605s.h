/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_GPU_GX6605S_H_
#define _DRIVER_GPU_GX6605S_H_

#include <bits.h>

enum GX6605Ss_vpu_registers {
    GX6605S_PP_CTRL                 = 0x00,
    GX6605S_PP_V_PHASE              = 0x04,
    GX6605S_PP_POSITION             = 0x08,
    GX6605S_PP_SOURCE_SIZE          = 0x0c,
    GX6605S_PP_VIEW_SIZE            = 0x10,
    GX6605S_PP_ZOOM                 = 0x14,
    GX6605S_PP_FRAME_STRIDE         = 0x18,
    GX6605S_PP_FILTER_SIGN          = 0x1c,
    GX6605S_PP_PHASE_0_H            = 0x20,
    GX6605S_PP_PHASE_0_V            = 0x24,
    GX6605S_PP_DISP_CTRL            = 0x28,
    GX6605S_PP_DISP_R_PTR           = 0x2c,
    GX6605S_PP_BACK_COLOR           = 0x30,

    GX6605S_PIC_CTRL                = 0x40,
    GX6605S_PIC_V_PHASE             = 0x44,
    GX6605S_PIC_POSITION            = 0x48,
    GX6605S_PIC_SOURCE_SIZE         = 0x4c,
    GX6605S_PIC_VIEW_SIZE           = 0x50,
    GX6605S_PIC_ZOOM                = 0x54,
    GX6605S_PIC_PARA                = 0x58,
    GX6605S_PIC_FILTER_SIGN         = 0x5c,
    GX6605S_PIC_PHASE_0_H           = 0x60,
    GX6605S_PIC_PHASE_0_V           = 0x64,
    GX6605S_PIC_Y_TOP_ADDR          = 0x68,
    GX6605S_PIC_Y_BOTTOM_ADDR       = 0x6c,
    GX6605S_PIC_UV_TOP_ADDR         = 0x70,
    GX6605S_PIC_UV_BOTTOM_ADDR      = 0x74,
    GX6605S_PIC_BACK_COLOR          = 0x78,

    GX6605S_OSD_CTRL                = 0x90,
    GX6605S_OSD_FIRST_HEAD_PTR      = 0x94,
    GX6605S_OSD_VIEW_SIZE           = 0x98,
    GX6605S_OSD_ZOOM                = 0x9c,
    GX6605S_OSD_COLOR_KEY           = 0xa0,
    GX6605S_OSD_ALPHA_5551          = 0xa4,
    GX6605S_OSD_PHASE_0             = 0xa8,
    GX6605S_OSD_POSITION            = 0xac,

    GX6605S_CAP_CTRL                = 0xb0,
    GX6605S_CAP_ADDR                = 0xb4,
    GX6605S_CAP_HEIGHT              = 0xb8,
    GX6605S_CAP_WIDTH               = 0xbc,

    GX6605S_VBI_CTRL                = 0xd0,
    GX6605S_VBI_FIRST_ADDR          = 0xd4,
    GX6605S_VBI_ADDR                = 0xd8,

    GX6605S_MIX_CTRL                = 0xdc,
    GX6605S_CHIPTEST                = 0xe0,
    GX6605S_SCAN_LINE               = 0xe4,
    GX6605S_SYS_PARA                = 0xe8,
    GX6605S_BUFF_CTRL1              = 0xec,
    GX6605S_BUFF_CTRL2              = 0xf0,
    GX6605S_EMPTY_GATE_1            = 0xf4,
    GX6605S_EMPTY_GATE_2            = 0xf8,
    GX6605S_FULL_GATE               = 0xfc,
    GX6605S_BUFFER_INT              = 0x100,

    GX6605S_PP_PARA_H               = 0x0200, /* 0x0200 ~ 0x02fc */
    GX6605S_PP_PARA_V               = 0x0300, /* 0x0300 ~ 0x03fc */

    GX6605S_OSD_PARA                = 0x0400, /* 0x0400 ~ 0x04fc */

    GX6605S_DISP0_CTRL              = 0x0500, /* 0x0500 ~ 0x051c */
    GX6605S_DISP1_CTRL              = 0x0520, /* 0x0520 ~ 0x053c */
    GX6605S_DISP2_CTRL              = 0x0540, /* 0x0540 ~ 0x055c */
    GX6605S_DISP3_CTRL              = 0x0560, /* 0x0560 ~ 0x057c */
    GX6605S_DISP4_CTRL              = 0x0580, /* 0x0580 ~ 0x059c */
    GX6605S_DISP5_CTRL              = 0x05a0, /* 0x05a0 ~ 0x05bc */
    GX6605S_DISP6_CTRL              = 0x05c0, /* 0x05c0 ~ 0x05dc */
    GX6605S_DISP7_CTRL              = 0x05e0, /* 0x05e0 ~ 0x05fc */

    GX6605S_PIC_PARA_H              = 0x0600, /* 0x0600 ~ 0x06fc */
    GX6605S_PIC_PARA_V              = 0x0700, /* 0x0700 ~ 0x07fc */
};

enum gx6605s_osd_region {
	GX6605S_OSDR_CTRL       = 0x00,
	GX6605S_OSDR_CLUT_PTR   = 0x04,
	GX6605S_OSDR_WIDTH      = 0x08,
	GX6605S_OSDR_HIGHT      = 0x0c,
	GX6605S_OSDR_FBADDR     = 0x10,
	GX6605S_OSDR_NEXT_PTR   = 0x14,
	GX6605S_OSDR_ALPHA      = 0x18,
};

/************************************************************************************************/
/*      Mnemonic                            value        meaning/usage                          */

/* OSD_CTRL bitflag definitions */
#define GX6605S_OSD_CTRL_EN                 BIT(0)
#define GX6605S_OSD_CTRL_ZOOM_MODE_EN_IPS   BIT(25)
#define GX6605S_OSD_CTRL_ANTI_FLICKER       BIT(29)
#define GX6605S_OSD_CTRL_ANTI_FLICKER_CBCR  BIT(31)

/* SYS_PARA bitflag definitions */
#define GX6605S_SYS_PARA_BYTESEQ_LOW        (0x03 << 20)
#define GX6605S_SYS_PARA_BYTESEQ_HIGH       (0x01 << 28)

/* OSD_CTRL bitflag definitions */
#define GX6605S_BUFF_CTRL2_REQ_LEN          (0x7ff << 0)
#define GX6605S_BUFF_CTRL2_STATE_DELAY      (0xff << 16)

/* OSD_POSITION bitflag definitions */
#define GX6605S_OSD_POSITION_X              (0x3FF << 0)
#define GX6605S_OSD_POSITION_Y              (0x3FF << 16)

/* OSDR_CTRL bitflag definitions */
#define GX6605S_OSDR_CTRL_CLUT_SWITCH       (0xff << 0)
#define GX6605S_OSDR_CTRL_CLUT_LENGTH       (0x03 << 8)
#define GX6605S_OSDR_CTRL_COLOR_MODE        (0x07 << 10)
#define GX6605S_OSDR_CTRL_CLUT_UPDATA_EN    (0x01 << 13)
#define GX6605S_OSDR_CTRL_FLIKER_FLITER_EN  (0x01 << 14)
#define GX6605S_OSDR_CTRL_COLOR_KEY_EN      (0x01 << 15)
#define GX6605S_OSDR_CTRL_MIX_WEIGHT        (0x7f << 16)
#define GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN   (0x01 << 23)
#define GX6605S_OSDR_CTRL_TRUE_COLOR_MODE   (0x03 << 24)
#define GX6605S_OSDR_CTRL_ARGB_CONVERT      (0x01 << 26)

/* OSDR_WIDTH bitflag definitions */
#define GX6605S_OSDR_WIDTH_LEFT             (0x7FF << 0)
#define GX6605S_OSDR_WIDTH_RIGHT            (0x7FF << 16)

/* OSDR_HIGHT bitflag definitions */
#define GX6605S_OSDR_HIGHT_TOP              (0x7FF << 0)
#define GX6605S_OSDR_HIGHT_BOTTOM           (0x7FF << 16)

/* OSDR_ALPHA bitflag definitions */
#define GX6605S_OSDR_BASELINE               (0x1fff << 0)
#define GX6605S_OSDR_ALPHA_RATIO            (0xff << 16)
#define GX6605S_OSDR_ALPHA_RATIO_EN         (0x01 << 24)
#define GX6605S_OSDR_LIST_END               (0x01 << 31)

#endif  /* _DRIVER_GPU_GX6605S_H_ */
