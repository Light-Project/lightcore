#ifndef _SUNXIH3_H_ 
#define _SUNXIH3_H_

#include <types.h>

/*
* SYS Register structure
*/

typedef struct{
    uint8_t  RESERVED0[24];     // 0x00 res
    uint32_t VER;               // 0x24 version
    uint8_t  RESERVED1[8];      // 0x28
    uint32_t EMAC_EPHY_CLK;     // 0x30 EMAC-EPHY Clock
} SYS_TypeDef;

#define SYS_BASE        ((uint32_t)0x01C00000)

#define SYS             ((SYS_TypeDef *)SYS_BASE)


/*
* CCU Register structure
*/

typedef struct{
    // Bias 0x000
    uint32_t PLL_CPUX_CTRL;
    // Bias 0x004
    uint32_t RESERVED1;
    // Bias 0x008
    uint32_t PLL_AUDIO_CTRL;
    // Bias 0x00c
    uint32_t RESERVED2;
    // Bias 0x010
    uint32_t PLL_VIDEO_CTRL;
    // Bias 0x014
    uint32_t RESERVED3;
    // Bias 0x018
    uint32_t PLL_VE_CTRL;
    // Bias 0x01c
    uint32_t RESERVED4;
    // Bias 0x020
    uint32_t PLL_DDR_CTRL;
    // Bias 0x024
    uint32_t RESERVED5;
    // Bias 0x028
    uint32_t PLL_PERIPH0_CTRL;
    // Bias 0x02c
    uint32_t RESERVED6;
    // Bias 0x030
    uint32_t RESERVED7;
    // Bias 0x034
    uint32_t RESERVED8;
    // Bias 0x038
    uint32_t PLL_GPU_CTRL;
    // Bias 0x03c
    uint32_t RESERVED9;
    // Bias 0x040
    uint32_t RESERVED10;
    // Bias 0x044
    uint32_t PLL_PERIPH1_CTRL;
    // Bias 0x048
    uint32_t PLL_DE_CTRL;
    // Bias 0x04c
    uint32_t RESERVED11;
    // Bias 0x050
    uint32_t CPUX_AXI_CFG;
    // Bias 0x054
    uint32_t AHB1_APB1_CFG;
    // Bias 0x058
    uint32_t APB2_CFG;
    // Bias 0x05c
    uint32_t AHB2_CFG;
    // Bias 0x060
    uint32_t BUS_CLK_GATING_REG0;
    // Bias 0x064
    uint32_t BUS_CLK_GATING_REG1;
    // Bias 0x068
    uint32_t BUS_CLK_GATING_REG2;
    // Bias 0x06c
    uint32_t BUS_CLK_GATING_REG3;
    // Bias 0x070
    uint32_t BUS_CLK_GATING_REG4;
    // Bias 0x074
    uint32_t THS_CLK;
    // Bias 0x078
    uint32_t RESERVED12;
    // Bias 0x07c
    uint32_t RESERVED13;
    // Bias 0x080
    uint32_t NAND_CLK;
    // Bias 0x084
    uint32_t RESERVED14;
    // Bias 0x088
    uint32_t SDMMC0_CLK;
    // Bias 0x08c
    uint32_t SDMMC1_CLK;
    // Bias 0x090
    uint32_t SDMMC2_CLK;
    // Bias 0x094
    uint32_t RESERVED15;
    // Bias 0x098
    uint32_t RESERVED16;
    // Bias 0x09c
    uint32_t CE_CLK;
    // Bias 0x09c
    uint32_t RESERVED17;
    // Bias 0x0a0
    uint32_t SPI0_CLK;
    // Bias 0x0a4
    uint32_t SPI1_CLK;
    // Bias 0x0a8
    uint32_t RESERVED18;
    // Bias 0x0ac
    uint32_t RESERVED19;
    // Bias 0x0b0
    uint32_t I2S_PCM0_CLK;
    // Bias 0x0b4
    uint32_t I2S_PCM1_CLK;
    // Bias 0x0b8
    uint32_t I2S_PCM2_CLK;
    // Bias 0x0bc
    uint32_t RESERVED20;
    // Bias 0x0c0
    uint32_t OWA_CLK;
    // Bias 0x0c4
    uint32_t RESERVED21;
    // Bias 0x0c8
    uint32_t RESERVED22;
    // Bias 0x0cc
    uint32_t USBPHY_CFG;
    // Bias 0x0d0
    uint32_t RESERVED23;
    // Bias 0x0d8
    uint32_t RESERVED24;
    // Bias 0x0dc
    uint32_t RESERVED25;
    // Bias 0x0e0
    uint32_t RESERVED26;
    // Bias 0x0e4
    uint32_t RESERVED27;
    // Bias 0x0e8
    uint32_t RESERVED28;
    // Bias 0x0ec
    uint32_t RESERVED29;
    // Bias 0x0f0
    uint32_t RESERVED30;
    // Bias 0x0f4
    uint32_t DRAM_CFG;
    // Bias 0x0f8
    uint32_t RESERVED31;
    // Bias 0x0fc
    uint32_t MBUS_RST;
    // Bias 0x100
    uint32_t DRAM_CLK_GATING;
    // Bias 0x104
    uint32_t RESERVED32;
    // Bias 0x108
    uint32_t RESERVED33;
    // Bias 0x10c
    uint32_t RESERVED34;
    // Bias 0x110
    uint32_t RESERVED35;
    // Bias 0x114
    uint32_t RESERVED36;
    // Bias 0x118
    uint32_t TCON0_CLK;
    // Bias 0x11c
    uint32_t RESERVED37;
    // Bias 0x120
    uint32_t TVE_CLK;
    // Bias 0x124
    uint32_t DEINTERLACE_CLK;
    // Bias 0x128
    uint32_t RESERVED38;
    // Bias 0x12c
    uint32_t RESERVED39;
    // Bias 0x130
    uint32_t CSI_MISC_CLK_;
    // Bias 0x134
    uint32_t CSI_CLK;
    // Bias 0x138
    uint32_t RESERVED40;
    // Bias 0x13c
    uint32_t VE_CLK;
    // Bias 0x140
    uint32_t AC_DIG_CLK;
    // Bias 0x144
    uint32_t AVS_CLK;
    // Bias 0x148
    uint32_t RESERVED41;
    // Bias 0x14c
    uint32_t RESERVED42;
    // Bias 0x150
    uint32_t HDMI_CLK;
    // Bias 0x154
    uint32_t HDMI_SLOW_CLK;
    // Bias 0x158
    uint32_t RESERVED43;
    // Bias 0x15c
    uint32_t MBUS_CLK;
    // Bias 0x160
    uint32_t RESERVED44;
    // Bias 0x164
    uint32_t RESERVED45;
    // Bias 0x168
    uint32_t RESERVED46;
    // Bias 0x16c
    uint32_t RESERVED47;
    // Bias 0x170
    uint32_t RESERVED48;
    // Bias 0x174
    uint32_t RESERVED49;
    // Bias 0x178
    uint32_t RESERVED50;
    // Bias 0x17c
    uint32_t RESERVED51;
    // Bias 0x180
    uint32_t RESERVED52;
    // Bias 0x184
    uint32_t RESERVED53;
    // Bias 0x188
    uint32_t RESERVED54;
    // Bias 0x18c
    uint32_t RESERVED55;
    // Bias 0x190
    uint32_t RESERVED56;
    // Bias 0x194
    uint32_t RESERVED57;
    // Bias 0x198
    uint32_t RESERVED58;
    // Bias 0x19c
    uint32_t RESERVED59;
    // Bias 0x1a0
    uint32_t GPU_CLK;
    // Bias 0x1a4
    uint32_t RESERVED60;
    // Bias 0x1a8
    uint32_t RESERVED61;
    // Bias 0x1ac
    uint32_t RESERVED62;
    // Bias 0x1b0
    uint32_t RESERVED63;
    // Bias 0x1b4
    uint32_t RESERVED64;
    // Bias 0x1b8
    uint32_t RESERVED65;
    // Bias 0x1bc
    uint32_t RESERVED66;
    // Bias 0x1c0
    uint32_t RESERVED67;
    // Bias 0x1c4
    uint32_t RESERVED68;
    // Bias 0x1c8
    uint32_t RESERVED69;
    // Bias 0x1cc
    uint32_t RESERVED70;
    // Bias 0x1d0
    uint32_t RESERVED71;
    // Bias 0x1d4
    uint32_t RESERVED72;
    // Bias 0x1d8
    uint32_t RESERVED73;
    // Bias 0x1dc
    uint32_t RESERVED74;
    // Bias 0x1e0
    uint32_t RESERVED75;
    // Bias 0x1e4
    uint32_t RESERVED76;
    // Bias 0x1e8
    uint32_t RESERVED77;
    // Bias 0x1ec
    uint32_t RESERVED78;
    // Bias 0x1f0
    uint32_t RESERVED79;
    // Bias 0x1f4
    uint32_t RESERVED80;
    // Bias 0x1f8
    uint32_t RESERVED81;
    // Bias 0x1fc
    uint32_t RESERVED82;
    // Bias 0x200
    uint32_t PLL_STABLE_TIME_REG0;
    // Bias 0x204
    uint32_t PLL_STABLE_TIME_REG1;
    // Bias 0x208
    uint32_t RESERVED83;
    // Bias 0x20c
    uint32_t RESERVED84;
    // Bias 0x210
    uint32_t RESERVED85;
    // Bias 0x214
    uint32_t RESERVED86;
    // Bias 0x218
    uint32_t RESERVED87;
    // Bias 0x21c
    uint32_t RESERVED88;
    // Bias 0x220
    uint32_t PLL_CPUX_BIAS;
    // Bias 0x224
    uint32_t PLL_AUDIO_BIAS;
    // Bias 0x228
    uint32_t PLL_VIDEO_BIAS;
    // Bias 0x22c
    uint32_t PLL_VE_BIAS;
    // Bias 0x230
    uint32_t PLL_DDR_BIAS;
    // Bias 0x234
    uint32_t PLL_PERIPH0_BIAS;
    // Bias 0x238
    uint32_t RESERVED89;
    // Bias 0x23c
    uint32_t PLL_GPU_BIAS;
    // Bias 0x240
    uint32_t RESERVED90;
    // Bias 0x244
    uint32_t PLL_PERIPH1_BIAS;
    // Bias 0x248
    uint32_t PLL_DE_BIAS;
    // Bias 0x24c
    uint32_t RESERVED91;
    // Bias 0x250
    uint32_t PLL_CPUX_TUN;
    // Bias 0x254
    uint32_t RESERVED92;
    // Bias 0x258
    uint32_t RESERVED93;
    // Bias 0x25c
    uint32_t RESERVED94;
    // Bias 0x260
    uint32_t PLL_DDR_TUN;
    // Bias 0x264
    uint32_t RESERVED95;
    // Bias 0x268
    uint32_t RESERVED96;
    // Bias 0x26c
    uint32_t RESERVED97;
    // Bias 0x270
    uint32_t RESERVED98;
    // Bias 0x274
    uint32_t RESERVED99;
    // Bias 0x278
    uint32_t RESERVED100;
    // Bias 0x27c
    uint32_t RESERVED101;
    // Bias 0x280
    uint32_t PLL_CPUX_PAT_CTRL;
    // Bias 0x284
    uint32_t PLL_AUDIO_PAT_CTRL;
    // Bias 0x288
    uint32_t PLL_VIDEO_PAT_CTRL;
    // Bias 0x28c
    uint32_t PLL_VE_PAT_CTRL;
    // Bias 0x290
    uint32_t PLL_DDR_PAT_CTRL;
    // Bias 0x294
    uint32_t RESERVED102;
    // Bias 0x298
    uint32_t RESERVED103;
    // Bias 0x29c
    uint32_t PLL_GPU_PAT_CTRL;
    // Bias 0x2a0
    uint32_t RESERVED104;
    // Bias 0x2a4
    uint32_t PLL_PERIPH1_PAT_CTRL;
    // Bias 0x2a8
    uint32_t PLL_DE_PAT_CTRL;
    // Bias 0x2ac
    uint32_t RESERVED105;
    // Bias 0x2b0
    uint32_t RESERVED106;
    // Bias 0x2b4
    uint32_t RESERVED107;
    // Bias 0x2b8
    uint32_t RESERVED108;
    // Bias 0x2bc
    uint32_t RESERVED109;
    // Bias 0x2c0
    uint32_t BUS_SOFT_RST_REG0;
    // Bias 0x2c4
    uint32_t BUS_SOFT_RST_REG1;
    // Bias 0x2c8
    uint32_t BUS_SOFT_RST_REG2;
    // Bias 0x2cc
    uint32_t RESERVED110;
    // Bias 0x2d0
    uint32_t BUS_SOFT_RST_REG3;
    // Bias 0x2d4
    uint32_t RESERVED111;
    // Bias 0x2d8
    uint32_t BUS_SOFT_RST_REG4;
    // Bias 0x2dc
    uint32_t RESERVED112;
    // Bias 0x2e0
    uint32_t RESERVED113;
    // Bias 0x2e4
    uint32_t RESERVED114;
    // Bias 0x2e8
    uint32_t RESERVED115;
    // Bias 0x2f0
    uint32_t CCU_SEC_SWITCH;
    // Bias 0x2f4
    uint32_t RESERVED116;
    // Bias 0x2f8
    uint32_t RESERVED117;
    // Bias 0x2fc
    uint32_t RESERVED118;
    // Bias 0x300
    uint32_t PS_CTRL;
    // Bias 0x304
    uint32_t PS_CNT;
} CCU_TypeDef;

#define CCU_BASE        ((uint32_t)0x01C20000)

#define CCU             ((CCU_TypeDef *)CCU_BASE)


/*
* TIMER Register structure
*/

typedef struct{
    uint32_t TMR_IRQ_EN;        // 0x00 irq enable
    uint32_t TMR_IRQ_STA;       // 0x04 status
    uint8_t  RESERVED0[8];      // 0x08 res
    uint32_t TMR0_CTRL;         // 0x10 timer0 ctl
    uint32_t TMR0_INTV_VALUE;   // 0x14 timer0 interval value
    uint32_t TMR0_CUR_VALUE;    // 0x18 timer0 current value
    uint8_t  RESERVED1[4];      // 0x1c res
    uint32_t TMR1_CTRL;         // 0x20 timer1 ctl
    uint32_t TMR1_INTV_VALUE;   // 0x24 timer1 interval value
    uint32_t TMR1_CUR_VALUE;    // 0x28 timer1 current value
    uint8_t  RESERVED2[44];     // 0x2c
    uint32_t AVS_CNT_CTL;       // 0x80 avs ctl
    uint32_t AVS_CNT0;          // 0x84 avs count0
    uint32_t AVS_CNT1;          // 0x88 avs count1
    uint32_t AVS_CNT_DIV;       // 0x8c avs clk div
    uint8_t  RESERVED3[16];     // 0x90
    uint32_t WDOG_IRQ_EN;       // 0xa0 wdt irq ctl
    uint32_t WDOG_IRQ_STA;      // 0xa4 wdt irq status
    uint8_t  RESERVED4[8];      // 0xa8
    uint32_t WDOG_CTRL;         // 0xb0 wdt ctl
    uint32_t WDOG_CFG;          // 0xb4 wdt Configuration
    uint32_t WDOG_MODE;         // 0xb8 wdt mode
} TIMER_TypeDef;

#define TIMER_BASE      ((uint32_t)0x01c20c00)

#define TIMER           ((TIMER_TypeDef *)TIMER_BASE)


/*
* GPIO Register structure
*/

typedef struct{
    uint32_t CFG0;
    uint32_t CFG1;
    uint32_t CFG2;
    uint32_t CFG3;
    uint32_t DATA;
    uint32_t DRV0;
    uint32_t DRV1;
    uint32_t PULL0;
    uint32_t PULL1;
} GPIO_TypeDef;

typedef struct{
    uint32_t CFG0;
    uint32_t CFG1;
    uint32_t CFG2;
    uint32_t CFG3;
    uint32_t CTL;
    uint32_t STA;
    uint32_t DEB;
} GPIO_INT_TypeDef;

#define GPIO_PIO_BASE   ((uint32_t)0x01c20800)

#define GPIOA_BASE      (GPIO_PIO_BASE + 0x00)
#define GPIOC_BASE      (GPIO_PIO_BASE + 0x48)
#define GPIOD_BASE      (GPIO_PIO_BASE + 0x6C)
#define GPIOE_BASE      (GPIO_PIO_BASE + 0x90)
#define GPIOF_BASE      (GPIO_PIO_BASE + 0xB4)
#define GPIOG_BASE      (GPIO_PIO_BASE + 0xD8)

#define GPIOA_INT_BASE  (GPIO_PIO_BASE + 0x200)
#define GPIOG_INT_BASE  (GPIO_PIO_BASE + 0x220)

#define PA              ((GPIO_TypeDef *)GPIOA_BASE)
#define PC              ((GPIO_TypeDef *)GPIOC_BASE)
#define PD              ((GPIO_TypeDef *)GPIOD_BASE)
#define PE              ((GPIO_TypeDef *)GPIOE_BASE)
#define PF              ((GPIO_TypeDef *)GPIOF_BASE)

#define PA_INT          ((GPIO_INT_TypeDef *)GPIOA_INT_BASE)
#define PG_INT          ((GPIO_INT_TypeDef *)GPIOG_INT_BASE)


/*
* UART Register structure
*/

typedef struct{
    // Bias 0x00
    uint32_t RBR_THR_DLL;
    // Bias 0x04
    uint32_t DLH_IER;
    // Bias 0x08
    uint32_t IIR_FCR;
    // Bias 0x0c
    uint32_t LCR;
    // Bias 0x10
    uint32_t MCR;
    // Bias 0x14
    uint32_t LSR;
    // Bias 0x18
    uint32_t MSR;
    // Bias 0x1c
    uint32_t SCH;
    // Bias 0x20
    uint8_t  RESERVED0[96];
    // Bias 0x7c
    uint32_t USR;
    // Bias 0x80
    uint32_t TFL;
    // Bias 0x84
    uint32_t RFL;
    // Bias 0x88
    uint8_t  RESERVED1[28];
    // Bias 0xa0
    uint32_t HALT;
} UART_TypeDef;

#define UART0_BASE      ((uint32_t)0x01C28000)
#define UART1_BASE      ((uint32_t)0x01C28400)
#define UART2_BASE      ((uint32_t)0x01C28800)
#define UART3_BASE      ((uint32_t)0x01C28C00)
#define UARTR_BASE      ((uint32_t)0x01F02800)

#define UART0           ((UART_TypeDef *)UART0_BASE)
#define UART1           ((UART_TypeDef *)UART1_BASE)
#define UART2           ((UART_TypeDef *)UART2_BASE)
#define UART3           ((UART_TypeDef *)UART3_BASE)
#define UARTR           ((UART_TypeDef *)UARTR_BASE)


#endif
