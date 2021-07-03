#ifndef _SUNXIF1_H_
#define _SUNXIF1_H_

#include <types.h>

/*
* DRAM Register structure
*/

typedef struct{
    uint32_t SCONR;
    uint32_t STMG0R;
    uint32_t STMG1R;
    uint32_t SCTLR;
    uint32_t SREFR;
    uint32_t SEXTMR;
    uint32_t DDLYR;
    uint32_t DADRR;
    uint32_t DVALR;
    uint32_t DRPTR0;
    uint32_t DRPTR1;
    uint32_t DRPTR2;
    uint32_t DRPTR3;
    uint32_t SEFR;
    uint32_t MAE;
    uint32_t ASPR;
    uint32_t SDLY0;
    uint32_t SDLY1;
    uint32_t SDLY2;
    uint32_t MCR0;
    uint32_t MCR1;
    uint32_t MCR2;
    uint32_t MCR3;
    uint32_t MCR4;
    uint32_t MCR5;
    uint32_t MCR6;
    uint32_t MCR7;
    uint32_t MCR8;
    uint32_t MCR9;
    uint32_t MCR10;
    uint32_t MCR11;
    uint32_t BWCR;
} __packed DRAMC_TypeDef;

#define DRAMC_BASE      ((uint32_t)0x01c01000)

#define CHIP_DRAMC      ((DRAMC_TypeDef *)DRAMC_BASE)

/*
* CCU Register structure
*/

typedef struct{
    uint32_t PLL_CPU_CTRL;          // 0x00 
    uint8_t  RESERVED1[4];          // 0x04
    uint32_t PLL_AUDIO_CTRL;        // 0x08
    uint8_t  RESERVED2[4];          // 0x0c
    uint32_t PLL_VIDEO_CTRL;        // 0x10
    uint8_t  RESERVED3[4];          // 0x14
    uint32_t PLL_VE_CTRL;           // 0x18
    uint8_t  RESERVED4[4];          // 0x1c
    uint32_t PLL_DDR_CTRL;          // 0x20
    uint8_t  RESERVED5[4];          // 0x24
    uint32_t PLL_PERIPH_CTRL;       // 0x28
    uint8_t  RESERVED6[36];         // 0x2c
    uint32_t CPU_CLK_SRC;           // 0x50    
    uint32_t AHB_APB_HCLKC_CFG;     // 0x54
    uint8_t  RESERVED7[8];          // 0x58
    uint32_t BUS_CLK_GATING_REG0;   // 0x60
    uint32_t BUS_CLK_GATING_REG1;   // 0x64
    uint32_t BUS_CLK_GATING_REG2;   // 0x68
    uint8_t  RESERVED8[28];         // 0x6c
    uint32_t SDMMC0_CLK;            // 0x88
    uint32_t SDMMC1_CLK;            // 0x8c
    uint8_t  RESERVED9[32];         // 0x90
    uint32_t DAUDIO_CLK;            // 0xb0
    uint32_t OWA_CLK;               // 0xb4
    uint32_t CIR_CLK;               // 0xb8
    uint8_t  RESERVED10[16];        // 0xbc
    uint32_t USBPHY_CLK;            // 0xcc
    uint8_t  RESERVED11[48];        // 0xd0
    uint32_t DRAM_GATING;           // 0x100
    uint32_t BE_CLK;                // 0x104
    uint8_t  RESERVED12[4];         // 0x108
    uint32_t FE_CLK;                // 0x10c
    uint8_t  RESERVED13[8];         // 0x110
    uint32_t TCON_CLK;              // 0x118
    uint32_t DI_CLK;                // 0x11c
    uint32_t TVE_CLK;               // 0x120
    uint32_t TVD_CLK;               // 0x124
    uint8_t  RESERVED14[12];        // 0x128
    uint32_t CSI_CLK;               // 0x134
    uint8_t  RESERVED15[4];         // 0x138
    uint32_t VE_CLK;                // 0x13c
    uint32_t AUDIO_CODEC_CLK;       // 0x140
    uint32_t AVS_CLK;               // 0x144
    uint8_t  RESERVED16[184];       // 0x148
    uint32_t PLL_STABLE_TIME_REG0;  // 0x200
    uint32_t PLL_STABLE_TIME_REG1;  // 0x204
    uint8_t  RESERVED17[24];        // 0x208
    uint32_t PLL_CPU_BIAS;          // 0x220
    uint32_t PLL_AUDIO_BIAS;        // 0x224
    uint32_t PLL_VIDEO_BIAS;        // 0x228
    uint32_t PLL_VE_BIAS;           // 0x22c
    uint32_t PLL_DDR_BIAS;          // 0x230
    uint32_t PLL_PERIPH_BIAS;       // 0x234
    uint8_t  RESERVED18[24];        // 0x238
    uint32_t PLL_CPU_TUN;           // 0x250
    uint8_t  RESERVED19[12];        // 0x254
    uint32_t PLL_DDR_TUN;           // 0x260
    uint8_t  RESERVED20[32];        // 0x264
    uint32_t PLL_AUDIO_PAT_CTRL;    // 0x284
    uint32_t PLL_VIDEO_PAT_CTRL;    // 0x288
    uint8_t  RESERVED21[4];         // 0x28c
    uint32_t PLL_DDR_PAT_CTRL;      // 0x290
    uint8_t  RESERVED22[44];        // 0x294
    uint32_t BUS_SOFT_RST_REG0;     // 0x2c0
    uint32_t BUS_SOFT_RST_REG1;     // 0x2c4
    uint8_t  RESERVED23[8];         // 0x2c8
    uint32_t BUS_SOFT_RST_REG2;     // 0x2d0

} __packed CCU_Typedef;

#define CCU_BASE        ((uint32_t)0x01c20000)

#define CHIP_CCU        ((CCU_Typedef *)CCU_BASE)

/*
* TIMER Register structure
*/

typedef struct{
    uint32_t TMR_IRQ_EN;        // 0x00
    uint32_t TMR_IRQ_STA;       // 0x04
    uint8_t  RESERVED0[8];      // 0x08
    uint32_t TMR0_CTRL;         // 0x10
    uint32_t TMR0_INTV_VALUE;   // 0x14
    uint32_t TMR0_CUR_VALUE;    // 0x18
    uint8_t  RESERVED1[4];      // 0x1c
    uint32_t TMR1_CTRL;         // 0x20
    uint32_t TMR1_INTV_VALUE;   // 0x24
    uint32_t TMR1_CUR_VALUE;    // 0x28
    uint8_t  RESERVED2[4];      // 0x2c
    uint32_t TMR2_CTRL;         // 0x30
    uint32_t TMR2_INTV_VALUE;   // 0x34
    uint32_t TMR2_CUR_VALUE;    // 0x38
    uint8_t  RESERVED3[68];     // 0x3c
    uint32_t AVS_CNT_CTL;       // 0x80
    uint32_t AVS_CNT0;          // 0x84
    uint32_t AVS_CNT1;          // 0x88
    uint32_t AVS_CNT_DIV;       // 0x8c
    uint8_t  RESERVED4[16];     // 0x90
    uint32_t WDOG_IRQ_EN;       // 0xa0
    uint32_t WDOG_IRQ_STA;      // 0xa4
    uint8_t  RESERVED5[8];      // 0xa8
    uint32_t WDOG_CTRL;         // 0xb0
    uint32_t WDOG_CFG;          // 0xb4
    uint32_t WDOG_MODE;         // 0xb8
} __packed TIMER_TypeDef;

#define TIMER_BASE      ((uint32_t)0x01c20c00)

#define CHIP_TIMER      ((TIMER_TypeDef *)TIMER_BASE)

/*
* PWM Register structure
*/

typedef struct{
    uint32_t CTRL;
    uint32_t CH0;
    uint32_t CH1;
} __packed PWM_TypeDef;

#define PWM_BASE        ((uint32_t)0x01c21000)

#define CHIP_PWM        ((PWM_TypeDef *)PWM_BASE)

/*
* INTC Register structure
*/

typedef struct{
    uint32_t VECTOR;            // 0x00
    uint32_t BASE_ADDR;         // 0x04
    uint8_t  RESERVED0[4];      // 0x08
    uint32_t NMI_INT_CTRL;      // 0x0c
    uint32_t PEND_REG0;         // 0x10
    uint32_t PEND_REG1;         // 0x14
    uint8_t  RESERVED1[8];      // 0x18
    uint32_t EN_REG0;           // 0x20
    uint32_t EN_REG1;           // 0x24
    uint8_t  RESERVED2[8];      // 0x28
    uint32_t MASK_REG0;         // 0x30
    uint32_t MASK_REG1;         // 0x34
    uint8_t  RESERVED3[8];      // 0x38
    uint32_t RESP_REG0;         // 0x40
    uint32_t RESP_REG1;         // 0x44
    uint8_t  RESERVED4[8];      // 0x48
    uint32_t FF_REG0;           // 0x50
    uint32_t FF_REG1;           // 0x54
    uint8_t  RESERVED5[8];      // 0x58
    uint32_t PRIO_REG0;         // 0x60
    uint32_t PRIO_REG1;         // 0x64
    uint32_t PRIO_REG2;         // 0x68
    uint32_t PRIO_REG3;         // 0x6c
} __packed INTC_TypeDef;

#define INTC_BASE       ((uint32_t)0x01C20400)

#define CHIP_INTC       ((INTC_TypeDef *)INTC_BASE)

/*
* DMA Register structure
*/

typedef struct{
    uint32_t DMA_INT_CTRL;
    uint32_t DMA_INT_STA;
    uint32_t DMA_PTY_CFG;
} __packed DMA_TypeDef;

typedef struct{
    uint32_t NDMA_CFG;
    uint32_t NDMA_SRC_ADR;
    uint32_t NDMA_DES_ADR;
    uint32_t NDMA_BYTE_CNT;
} __packed NDMA_TypeDef;

typedef struct{
    uint32_t DDMA_CFG;
    uint32_t DDMA_SRC_ADR;
    uint32_t DDMA_DES_ADR;
    uint32_t DDMA_BYTE_CNT;
    uint32_t DDMA_PAR;
    uint32_t DDMA_GEN_DATA;
} __packed DDMA_TypeDef;

#define DMA_BASE        ((uint32_t)0x01C02000)

#define NDMA0_BASE      (DMA_BASE + 0x100+0x00)
#define NDMA1_BASE      (DMA_BASE + 0x100+0x20)
#define NDMA2_BASE      (DMA_BASE + 0x100+0x40)
#define NDMA3_BASE      (DMA_BASE + 0x100+0x80)

#define DDMA0_BASE      ((uint32_t)0x01C02000)
#define DDMA1_BASE      ((uint32_t)0x01C02000)
#define DDMA2_BASE      ((uint32_t)0x01C02000)
#define DDMA3_BASE      ((uint32_t)0x01C02000)

#define CHIP_DMA        ((DMA_TypeDef *)DMA_BASE)

#define CHIP_NDMA0      ((NDMA_TypeDef *)NDMA0_BASE)
#define CHIP_NDMA1      ((NDMA_TypeDef *)NDMA1_BASE)
#define CHIP_NDMA2      ((NDMA_TypeDef *)NDMA2_BASE)
#define CHIP_NDMA3      ((NDMA_TypeDef *)NDMA3_BASE)

#define CHIP_DDMA0      ((DDMA_TypeDef *)DDMA0_BASE)
#define CHIP_DDMA1      ((DDMA_TypeDef *)DDMA1_BASE)
#define CHIP_DDMA2      ((DDMA_TypeDef *)DDMA2_BASE)
#define CHIP_DDMA3      ((DDMA_TypeDef *)DDMA3_BASE)

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

typedef struct{
    uint32_t DRV;
    uint32_t PUL;
} GPIOF_SDR_TypeDef;

#define GPIO_PIO_BASE   ((uint32_t)0x01c20800)

#define GPIOA_BASE      (GPIO_PIO_BASE + 0x00)
#define GPIOB_BASE      (GPIO_PIO_BASE + 0x24)
#define GPIOC_BASE      (GPIO_PIO_BASE + 0x48)
#define GPIOD_BASE      (GPIO_PIO_BASE + 0x6C)
#define GPIOE_BASE      (GPIO_PIO_BASE + 0x90)
#define GPIOF_BASE      (GPIO_PIO_BASE + 0xB4)

#define GPIOD_INT_BASE  (GPIO_PIO_BASE + 0x200)
#define GPIOE_INT_BASE  (GPIO_PIO_BASE + 0x220)
#define GPIOF_INT_BASE  (GPIO_PIO_BASE + 0x240)

#define GPIO_SDR_BASE   (GPIO_PIO_BASE + 0x2C0)

#define CHIP_PA         ((GPIO_TypeDef *)GPIOA_BASE)
#define CHIP_PC         ((GPIO_TypeDef *)GPIOC_BASE)
#define CHIP_PD         ((GPIO_TypeDef *)GPIOD_BASE)
#define CHIP_PE         ((GPIO_TypeDef *)GPIOE_BASE)
#define CHIP_PF         ((GPIO_TypeDef *)GPIOF_BASE)

#define CHIP_PA_INT     ((GPIO_INT_TypeDef *)GPIOA_INT_BASE)
#define CHIP_PG_INT     ((GPIO_INT_TypeDef *)GPIOG_INT_BASE)

#define CHIP_SDR_PAD    ((GPIOF_SDR_TypeDef *)GPIO_SDR_BASE)

/*
* KEYADC Register structure
*/

typedef struct{
    uint32_t CTRL;
    uint32_t INTC;
    uint32_t INTS;
    uint32_t DATA;
} KEYADC_TypeDef;

#define KEYADC_BASE     ((uint32_t)0x01C23400)

#define CHIP_KEYADC     ((KEYADC_TypeDef *)KEYADC_BASE)

/*
* TP Register structure
*/

typedef struct{
    uint32_t CTRL_REG0;     // 0x00 tp ctl0
    uint32_t CTRL_REG1;     // 0x04 tp ctl1
    uint32_t CTRL_REG2;     // 0x08 tp ctl2
    uint32_t CTRL_REG3;     // 0x0c tp ctl3
    uint32_t FIFO_CTRL;     // 0x10 fifo ctl
    uint32_t FIFO_STAT;     // 0x14 fifo status
    uint8_t  RESERVED0[4];  // 0x18 res
    uint32_t COM_DATA;      // 0x1c common data
    uint8_t  RESERVED1[4];  // 0x20 res
    uint32_t DATA;          // 0x24 data buffer
} TP_TypeDef;

#define TP_BASE         ((uint32_t)0x01C24800)

#define CHIP_TP         ((TP_TypeDef*)TP_BASE)

/*
* CODEC Register structure
*/

typedef struct{
    uint32_t AC_DAC_DPC;                // 0x00
    uint32_t AC_DAC_FIFOC;              // 0x04
    uint32_t AC_DAC_FIFOS;              // 0x08
    uint32_t AC_DAC_TXDATA;             // 0x0c
    uint32_t AC_ADC_FIFOC;              // 0x10
    uint32_t AC_ADC_FIFOS;              // 0x14
    uint32_t AC_ADC_RXDATA;             // 0x18
    uint8_t  RESERVED0[4];              // 0x1c res
    uint32_t DAC_MIXER_CTRL;            // 0x20 
    uint32_t ADC_MIXER_CTRL;            // 0x24
    uint32_t ADDA_TUNE_REG;             // 0x28
    uint32_t BIAS_DA16_CAL_CTRL_REG0;   // 0x2c
} CODEC_TypeDef;

#define CODEC_BASE      ((uint32_t)0x01C23C00)

#define CHIP_CODEC      ((CODEC_TypeDef*)CODEC_BASE)

/*
* TWI Register structure
*/

typedef struct{
    uint32_t ADDR;          // 0x00 slave addr
    uint32_t XADDR;         // 0x04 ext slave addr
    uint32_t DATA;          // 0x08 data buffer
    uint32_t CNTR;          // 0x0c twi ctl
    uint32_t STAT;          // 0x10 twi status
    uint32_t CCR;           // 0x14 twi feq
    uint32_t SRST;          // 0x18 soft rst
    uint32_t EFR;           // 0x1c ext ctl
    uint32_t LCR;           // 0x20 line ctl
} TWI_TypeDef;

#define TWI0_BASE           ((uint32_t)0x01c27000)
#define TWI1_BASE           ((uint32_t)0x01c27400)
#define TWI2_BASE           ((uint32_t)0x01c27800)

#define CHIP_TWI0           ((TWI_TypeDef *)TWI0_BASE)
#define CHIP_TWI1           ((TWI_TypeDef *)TWI1_BASE)
#define CHIP_TWI2           ((TWI_TypeDef *)TWI2_BASE)


/*
* SPI Register structure
*/

typedef struct{
    uint8_t  RESERVED0[4];  // 0x00 res
    uint32_t GCR;           // 0x04 spi ctl
    uint32_t TCR;           // 0x08 tx ctl
    uint8_t  RESERVED1[4];  // 0x0c res
    uint32_t IER;           // 0x10 int ctl
    uint32_t ISR;           // 0x14 int status 
    uint32_t FCR;           // 0x18 fifo ctl
    uint32_t FSR;           // 0x1c fifo status
    uint32_t WCR;           // 0x20 wait clock
    uint32_t CCR;           // 0x24 feq set
    uint8_t  RESERVED2[8];  // 0x28 res
    uint32_t MBC;           // 0x30 burst count
    uint32_t MTC;           // 0x34 tx count
    uint32_t BCC;           // 0x38 burst ctl
    uint8_t  RESERVED3[452];// 0x3c res
    uint32_t TXD;           // 0x200 tx buffer
    uint8_t  RESERVED4[252];// 0x204 res
    uint32_t RXD;           // 0x300 rx buffer
} SPI_TypeDef;

#define SPI0_BASE       ((uint32_t)0x01c05000)
#define SPI1_BASE       ((uint32_t)0x01c06000)

#define CHIP_SPI0       ((SPI_TypeDef *)SPI0_BASE)
#define CHIP_SPI1       ((SPI_TypeDef *)SPI1_BASE)


/*
* UART Register structure
*/

typedef struct{
    uint32_t RBR_THR_DLL;   // 0x00 buffer and feq-low
    uint32_t DLH_IER;       // 0x04 feq-high and int-ctl
    uint32_t IIR_FCR;       // 0x08 int-ide and fifo-ctl
    uint32_t LCR;           // 0x0c line-ctl
    uint32_t MCR;           // 0x10 modem-ctl
    uint32_t LSR;           // 0x14 line-status
    uint32_t MSR;           // 0x18 modem-status
    uint32_t SCH;           // 0x1c cache
    uint8_t  RESERVED0[92]; // 0x20-0x7b res
    uint32_t USR;           // 0x7c uart status
    uint32_t TFL;           // 0x80 tx-fifo level
    uint32_t RFL;           // 0x84 rx-fifo level
    uint8_t  RESERVED1[28]; // 0x88 res
    uint32_t HALT;          // 0xa4 dma handshake
    uint8_t  RESERVED2[8];  // 0xa8 res
    uint32_t DBG_DLL;       // 0xb0 debug dll
    uint32_t DBG_DLH;       // 0xb4 debug dlh
} UART_TypeDef;

#define UART0_BASE      ((uint32_t)0x01C25000)
#define UART1_BASE      ((uint32_t)0x01C25400)
#define UART2_BASE      ((uint32_t)0x01C25800)

#define CHIP_UART0      ((UART_TypeDef *)UART0_BASE)
#define CHIP_UART1      ((UART_TypeDef *)UART1_BASE)
#define CHIP_UART2      ((UART_TypeDef *)UART2_BASE)

#endif
