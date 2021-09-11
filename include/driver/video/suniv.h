/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_SUNIV_H_
#define _DRIVER_VIDEO_SUNIV_H_

#include <bits.h>

enum suniv_tcon_registers {
    SUNIV_TCON_CRTL             = 0x0,
    SUNIV_TCON_INT0             = 0x4,
    SUNIV_TCON_INT1             = 0x8,
    SUNIV_TCON_FRM_CTRL         = 0x10,
    SUNIV_TCON_FRM_SEED0_R      = 0x14,
    SUNIV_TCON_FRM_SEED0_G      = 0x18,
    SUNIV_TCON_FRM_SEED0_B      = 0x1c,
    SUNIV_TCON_FRM_SEED1_R      = 0x20,
    SUNIV_TCON_FRM_SEED1_G      = 0x24,
    SUNIV_TCON_FRM_SEED1_B      = 0x28,
    SUNIV_TCON_FRM_TBL0         = 0x2c,
    SUNIV_TCON_FRM_TBL1         = 0x30,
    SUNIV_TCON_FRM_TBL2         = 0x34,
    SUNIV_TCON_FRM_TBL3         = 0x38,

    SUNIV_TCON0_CRTL            = 0x40,
    SUNIV_TCON_CLK_CTRL         = 0x44,
    SUNIV_TCON0_BASEC_TIM0      = 0x48,
    SUNIV_TCON0_BASEC_TIM1      = 0x4c,
    SUNIV_TCON0_BASEC_TIM2      = 0x50,
    SUNIV_TCON0_BASEC_TIM3      = 0x54,
    SUNIV_TCON0_HW_TIM          = 0x58,
    SUNIV_TCON0_CPU_IF          = 0x60,
    SUNIV_TCON0_CPU_WR          = 0x64,
    SUNIV_TCON0_CPU_RD          = 0x68,
    SUNIV_TCON0_CPU_RD_NX       = 0x6c,
    SUNIV_TCON0_IOCTL0          = 0x88,
    SUNIV_TCON0_IOCTL1          = 0x8c,

    SUNIV_TCON1_CRTL            = 0x90,
    SUNIV_TCON1_BASEC_TIM0      = 0x94,
    SUNIV_TCON1_BASEC_TIM1      = 0x98,
    SUNIV_TCON1_BASEC_TIM2      = 0x9c,
    SUNIV_TCON1_BASEC_TIM3      = 0xa0,
    SUNIV_TCON1_BASEC_TIM4      = 0xa4,
    SUNIV_TCON1_BASEC_TIM5      = 0xa8,
    SUNIV_TCON1_IOCTL0          = 0xf0,
    SUNIV_TCON1_IOCTL1          = 0xf4,

    SUNIV_TCON_DEBUG            = 0xfc,
};

enum suniv_tve_registers {
    SUNIV_TVE_EN                = 0x00,
    SUNIV_TVE_CFG0              = 0x04,
    SUNIV_TVE_DAC0              = 0x08,
    SUNIV_TVE_CHROMA_FREQ       = 0x10,
    SUNIV_TVE_PORCH             = 0x14,
    SUNIV_TVE_VSYNC             = 0x18,
    SUNIV_TVE_LINE              = 0x1c,
    SUNIV_TVE_LEVEL             = 0x20,
    SUNIV_TVE_DAC1              = 0x24,
    SUNIV_TVE_DETECT_EN         = 0x30,
    SUNIV_TVE_DETECT_INTS       = 0x34,
    SUNIV_TVE_DETECT_STATUS     = 0x38,
    SUNIV_TVE_REG               = 0x12c,
    SUNIV_TVE_CFG1              = 0x138,
    SUNIV_TVE_CFG1              = 0x138,
    SUNIV_TVE_CFG2              = 0x13c,
};

enum suniv_debe_registers {
    SUNIV_DEBE_MODE_CTRL        = 0x800,
    SUNIV_DEBE_COLOR_CTRL       = 0x804,

    SUNIV_DEBE_LAY0_SIZE        = 0x810,
    SUNIV_DEBE_LAY1_SIZE        = 0x814,
    SUNIV_DEBE_LAY2_SIZE        = 0x818,
    SUNIV_DEBE_LAY3_SIZE        = 0x81c,

    SUNIV_DEBE_LAY0_CODNT       = 0x820,
    SUNIV_DEBE_LAY1_CODNT       = 0x824,
    SUNIV_DEBE_LAY2_CODNT       = 0x828,
    SUNIV_DEBE_LAY3_CODNT       = 0x82c,

    SUNIV_DEBE_LAY0_LINEWIDTH   = 0x840,
    SUNIV_DEBE_LAY1_LINEWIDTH   = 0x844,
    SUNIV_DEBE_LAY2_LINEWIDTH   = 0x848,
    SUNIV_DEBE_LAY3_LINEWIDTH   = 0x84c,

    SUNIV_DEBE_LAY0_FB_ADDR     = 0x850,
    SUNIV_DEBE_LAY1_FB_ADDR     = 0x854,
    SUNIV_DEBE_LAY2_FB_ADDR     = 0x858,
    SUNIV_DEBE_LAY3_FB_ADDR     = 0x85c,

    SUNIV_DEBE_REGBUFF_CTRL     = 0x870,

    SUNIV_DEBE_CK_MAX           = 0x880,
    SUNIV_DEBE_CK_MIN           = 0x884,
    SUNIV_DEBE_CK_CFG           = 0x888,

    SUNIV_DEBE_LAY0_ATT_CTRL0   = 0x890,
    SUNIV_DEBE_LAY1_ATT_CTRL0   = 0x894,
    SUNIV_DEBE_LAY2_ATT_CTRL0   = 0x898,
    SUNIV_DEBE_LAY3_ATT_CTRL0   = 0x89c,
    SUNIV_DEBE_LAY0_ATT_CTRL1   = 0x8a0,
    SUNIV_DEBE_LAY1_ATT_CTRL1   = 0x8a4,
    SUNIV_DEBE_LAY2_ATT_CTRL1   = 0x8a8,
    SUNIV_DEBE_LAY3_ATT_CTRL1   = 0x8ac,

    SUNIV_DEBE_HWC_CTRL         = 0x8d8,
    SUNIV_DEBE_HWCFB_CTRL       = 0x8f0,

    SUNIV_DEBE_WB_CTRL          = 0x8f0,
    SUNIV_DEBE_WB_ADDR          = 0x8f4,
    SUNIV_DEBE_WB_LW            = 0x8f8,

    SUNIV_DEBE_IYUV_CH_CTRL     = 0x920,
    SUNIV_DEBE_CH0_YUV_FB_ADDR  = 0x930,
    SUNIV_DEBE_CH1_YUV_FB_ADDR  = 0x934,
    SUNIV_DEBE_CH2_YUV_FB_ADDR  = 0x938,

    SUNIV_DEBE_CH0_YUV_BLW      = 0x940,
    SUNIV_DEBE_CH1_YUV_BLW      = 0x944,
    SUNIV_DEBE_CH2_YUV_BLW      = 0x948,

    SUNIV_DEBE_COEF00           = 0x950,
    SUNIV_DEBE_COEF01           = 0x954,
    SUNIV_DEBE_COEF02           = 0x958,
    SUNIV_DEBE_COEF03           = 0x95c,
    SUNIV_DEBE_COEF10           = 0x960,
    SUNIV_DEBE_COEF11           = 0x964,
    SUNIV_DEBE_COEF12           = 0x968,
    SUNIV_DEBE_COEF13           = 0x96c,
    SUNIV_DEBE_COEF20           = 0x970,
    SUNIV_DEBE_COEF21           = 0x974,
    SUNIV_DEBE_COEF22           = 0x978,
    SUNIV_DEBE_COEF23           = 0x97c,
};

enum suniv_defe_registers {
    SUNIV_DEFE_EN               = 0x0,
    SUNIV_DEFE_FRM_CTRL         = 0x4,
    SUNIV_DEFE_BYPASS           = 0x8,
    SUNIV_DEFE_AGTH_SEL         = 0xc,
    SUNIV_DEFE_LINT_CTRL        = 0x10,
    SUNIV_DEFE_BUF_ADDR0        = 0x20,
    SUNIV_DEFE_BUF_ADDR1        = 0x24,
    SUNIV_DEFE_BUF_ADDR2        = 0x28,
    SUNIV_DEFE_FIELD_CTRL       = 0x2c,
    SUNIV_DEFE_TB_OFF0          = 0x30,
    SUNIV_DEFE_TB_OFF1          = 0x34,
    SUNIV_DEFE_TB_OFF2          = 0x38,
    SUNIV_DEFE_LINESTRD0        = 0x40,
    SUNIV_DEFE_LINESTRD1        = 0x44,
    SUNIV_DEFE_LINESTRD2        = 0x48,
    SUNIV_DEFE_INPUT_FMT        = 0x4c,
    SUNIV_DEFE_WB_ADDR          = 0x50,
    SUNIV_DEFE_OUTPUT_FMT       = 0x5c,
    SUNIV_DEFE_INT_EN           = 0x60,
    SUNIV_DEFE_INT_STATUS       = 0x64,
    SUNIV_DEFE_STATUS           = 0x68,
    SUNIV_DEFE_CSC_COEF00       = 0x70,
    SUNIV_DEFE_CSC_COEF01       = 0x74,
    SUNIV_DEFE_CSC_COEF02       = 0x78,
    SUNIV_DEFE_CSC_COEF03       = 0x7c,
    SUNIV_DEFE_CSC_COEF10       = 0x80,
    SUNIV_DEFE_CSC_COEF11       = 0x84,
    SUNIV_DEFE_CSC_COEF12       = 0x88,
    SUNIV_DEFE_CSC_COEF13       = 0x8c,
    SUNIV_DEFE_CSC_COEF20       = 0x90,
    SUNIV_DEFE_CSC_COEF21       = 0x94,
    SUNIV_DEFE_CSC_COEF22       = 0x98,
    SUNIV_DEFE_CSC_COEF23       = 0x9c,
    SUNIV_DEFE_INSIZE           = 0x100,
    SUNIV_DEFE_OUTSIZE          = 0x104,
    SUNIV_DEFE_HORZFACT         = 0x108,
    SUNIV_DEFE_VERTFACT         = 0x10c,
    SUNIV_DEFE_CH0_HORZCOEF     = 0x400,   /* 0x400 + N * 4 */
    SUNIV_DEFE_CH0_VERTCOEF     = 0x500,   /* 0x400 + N * 4 */
    SUNIV_DEFE_CH1_HORZCOEF     = 0x600,   /* 0x400 + N * 4 */
    SUNIV_DEFE_CH1_VERTCOEF     = 0x700,   /* 0x400 + N * 4 */
};

#endif  /* _DRIVER_VIDEO_SUNIV_H_ */
