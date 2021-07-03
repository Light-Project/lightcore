 

#define ES_REG_CONTROL	0x00	/* R/W: Interrupt/Chip select control register */
#define ES_REG_STATUS	0x04	/* R/O: Interrupt/Chip select status register */
#define ES_REG_UART_DATA 0x08	/* R/W: UART data register */
#define ES_REG_UART_STATUS 0x09	/* R/O: UART status register */
#define ES_REG_UART_CONTROL 0x09	/* W/O: UART control register */

/* ES_REG_CONTROL */
#define   ES_1370_ADC_STOP	(1<<31)		/* disable capture buffer transfers */
#define   ES_1370_XCTL1 	(1<<30)		/* general purpose output bit */
#define   ES_1373_BYPASS_P1	(1<<31)		/* bypass SRC for PB1 */
#define   ES_1373_BYPASS_P2	(1<<30)		/* bypass SRC for PB2 */
#define   ES_1373_BYPASS_R	(1<<29)		/* bypass SRC for REC */
#define   ES_1373_TEST_BIT	(1<<28)		/* should be set to 0 for normal operation */
#define   ES_1373_RECEN_B	(1<<27)		/* mix record with playback for I2S/SPDIF out */
#define   ES_1373_SPDIF_THRU	(1<<26)		/* 0 = SPDIF thru mode, 1 = SPDIF == dig out */
#define   ES_1371_JOY_ASEL(o)	(((o)&0x03)<<24)/* joystick port mapping */
#define   ES_1371_JOY_ASELM	(0x03<<24)	/* mask for above */
#define   ES_1371_JOY_ASELI(i)  (((i)>>24)&0x03)
#define   ES_1371_GPIO_IN(i)	(((i)>>20)&0x0f)/* GPIO in [3:0] pins - R/O */
#define   ES_1370_PCLKDIVO(o)	(((o)&0x1fff)<<16)/* clock divide ratio for DAC2 */
#define   ES_1370_PCLKDIVM	((0x1fff)<<16)	/* mask for above */
#define   ES_1370_PCLKDIVI(i)	(((i)>>16)&0x1fff)/* clock divide ratio for DAC2 */
#define   ES_1371_GPIO_OUT(o)	(((o)&0x0f)<<16)/* GPIO out [3:0] pins - W/R */
#define   ES_1371_GPIO_OUTM     (0x0f<<16)	/* mask for above */
#define   ES_MSFMTSEL		(1<<15)		/* MPEG serial data format; 0 = SONY, 1 = I2S */
#define   ES_1370_M_SBB		(1<<14)		/* clock source for DAC - 0 = clock generator; 1 = MPEG clocks */
#define   ES_1371_SYNC_RES	(1<<14)		/* Warm AC97 reset */
#define   ES_1370_WTSRSEL(o)	(((o)&0x03)<<12)/* fixed frequency clock for DAC1 */
#define   ES_1370_WTSRSELM	(0x03<<12)	/* mask for above */
#define   ES_1371_ADC_STOP	(1<<13)		/* disable CCB transfer capture information */
#define   ES_1371_PWR_INTRM	(1<<12)		/* power level change interrupts enable */
#define   ES_1370_DAC_SYNC	(1<<11)		/* DAC's are synchronous */
#define   ES_1371_M_CB		(1<<11)		/* capture clock source; 0 = AC'97 ADC; 1 = I2S */
#define   ES_CCB_INTRM		(1<<10)		/* CCB voice interrupts enable */
#define   ES_1370_M_CB		(1<<9)		/* capture clock source; 0 = ADC; 1 = MPEG */
#define   ES_1370_XCTL0		(1<<8)		/* generap purpose output bit */
#define   ES_1371_PDLEV(o)	(((o)&0x03)<<8)	/* current power down level */
#define   ES_1371_PDLEVM	(0x03<<8)	/* mask for above */
#define   ES_BREQ		(1<<7)		/* memory bus request enable */
#define   ES_DAC1_EN		(1<<6)		/* DAC1 playback channel enable */
#define   ES_DAC2_EN		(1<<5)		/* DAC2 playback channel enable */
#define   ES_ADC_EN		(1<<4)		/* ADC capture channel enable */
#define   ES_UART_EN		(1<<3)		/* UART enable */
#define   ES_JYSTK_EN		(1<<2)		/* Joystick module enable */
#define   ES_1370_CDC_EN	(1<<1)		/* Codec interface enable */
#define   ES_1371_XTALCKDIS	(1<<1)		/* Xtal clock disable */
#define   ES_1370_SERR_DISABLE	(1<<0)		/* PCI serr signal disable */
#define   ES_1371_PCICLKDIS     (1<<0)		/* PCI clock disable */

/* ES_REG_STATUS */
#define   ES_INTR               (1<<31)		/* Interrupt is pending */
#define   ES_1371_ST_AC97_RST	(1<<29)		/* CT5880 AC'97 Reset bit */
#define   ES_1373_REAR_BIT27	(1<<27)		/* rear bits: 000 - front, 010 - mirror, 101 - separate */
#define   ES_1373_REAR_BIT26	(1<<26)
#define   ES_1373_REAR_BIT24	(1<<24)
#define   ES_1373_GPIO_INT_EN(o)(((o)&0x0f)<<20)/* GPIO [3:0] pins - interrupt enable */
#define   ES_1373_SPDIF_EN	(1<<18)		/* SPDIF enable */
#define   ES_1373_SPDIF_TEST	(1<<17)		/* SPDIF test */
#define   ES_1371_TEST          (1<<16)		/* test ASIC */
#define   ES_1373_GPIO_INT(i)	(((i)&0x0f)>>12)/* GPIO [3:0] pins - interrupt pending */
#define   ES_1370_CSTAT		(1<<10)		/* CODEC is busy or register write in progress */
#define   ES_1370_CBUSY         (1<<9)		/* CODEC is busy */
#define   ES_1370_CWRIP		(1<<8)		/* CODEC register write in progress */
#define   ES_1371_SYNC_ERR	(1<<8)		/* CODEC synchronization error occurred */
#define   ES_1371_VC(i)         (((i)>>6)&0x03)	/* voice code from CCB module */
#define   ES_1370_VC(i)		(((i)>>5)&0x03)	/* voice code from CCB module */
#define   ES_1371_MPWR          (1<<5)		/* power level interrupt pending */
#define   ES_MCCB		(1<<4)		/* CCB interrupt pending */
#define   ES_UART		(1<<3)		/* UART interrupt pending */
#define   ES_DAC1		(1<<2)		/* DAC1 channel interrupt pending */
#define   ES_DAC2		(1<<1)		/* DAC2 channel interrupt pending */
#define   ES_ADC		(1<<0)		/* ADC channel interrupt pending */

/* ES_REG_UART_STATUS */
#define   ES_RXINT		(1<<7)		/* RX interrupt occurred */
#define   ES_TXINT		(1<<2)		/* TX interrupt occurred */
#define   ES_TXRDY		(1<<1)		/* transmitter ready */
#define   ES_RXRDY		(1<<0)		/* receiver ready */


/* ES_REG_UART_CONTROL */
#define   ES_RXINTEN		(1<<7)		/* RX interrupt enable */
#define   ES_TXINTENO(o)	(((o)&0x03)<<5)	/* TX interrupt enable */
#define   ES_TXINTENM		(0x03<<5)	/* mask for above */
#define   ES_TXINTENI(i)	(((i)>>5)&0x03)
#define   ES_CNTRL(o)		(((o)&0x03)<<0)	/* control */
#define   ES_CNTRLM		(0x03<<0)	/* mask for above */

#define ES_REG_UART_RES	0x0a	/* R/W: UART reserver register */
#define   ES_TEST_MODE		(1<<0)		/* test mode enabled */

#define ES_REG_MEM_PAGE	0x0c	/* R/W: Memory page register */
#define   ES_MEM_PAGEO(o)	(((o)&0x0f)<<0)	/* memory page select - out */
#define   ES_MEM_PAGEM		(0x0f<<0)	/* mask for above */
#define   ES_MEM_PAGEI(i)	(((i)>>0)&0x0f) /* memory page select - in */

#define ES_REG_1370_CODEC 0x10	/* W/O: Codec write register address */
#define   ES_1370_CODEC_WRITE(a,d) ((((a)&0xff)<<8)|(((d)&0xff)<<0))

#define ES_REG_1371_CODEC 0x14	/* W/R: Codec Read/Write register address */
#define   ES_1371_CODEC_RDY	   (1<<31)	/* codec ready */
#define   ES_1371_CODEC_WIP	   (1<<30)	/* codec register access in progress */
#define   EV_1938_CODEC_MAGIC	   (1<<26)
#define   ES_1371_CODEC_PIRD	   (1<<23)	/* codec read/write select register */
#define   ES_1371_CODEC_WRITE(a,d) ((((a)&0x7f)<<16)|(((d)&0xffff)<<0))
#define   ES_1371_CODEC_READS(a)   ((((a)&0x7f)<<16)|ES_1371_CODEC_PIRD)
#define   ES_1371_CODEC_READ(i)    (((i)>>0)&0xffff)

#define ES_REG_1371_SMPRATE 0x10	/* W/R: Codec rate converter interface register */
#define   ES_1371_SRC_RAM_ADDRO(o) (((o)&0x7f)<<25)/* address of the sample rate converter */
#define   ES_1371_SRC_RAM_ADDRM	   (0x7f<<25)	/* mask for above */
#define   ES_1371_SRC_RAM_ADDRI(i) (((i)>>25)&0x7f)/* address of the sample rate converter */
#define   ES_1371_SRC_RAM_WE	   (1<<24)	/* R/W: read/write control for sample rate converter */
#define   ES_1371_SRC_RAM_BUSY     (1<<23)	/* R/O: sample rate memory is busy */
#define   ES_1371_SRC_DISABLE      (1<<22)	/* sample rate converter disable */
#define   ES_1371_DIS_P1	   (1<<21)	/* playback channel 1 accumulator update disable */
#define   ES_1371_DIS_P2	   (1<<20)	/* playback channel 1 accumulator update disable */
#define   ES_1371_DIS_R1	   (1<<19)	/* capture channel accumulator update disable */
#define   ES_1371_SRC_RAM_DATAO(o) (((o)&0xffff)<<0)/* current value of the sample rate converter */
#define   ES_1371_SRC_RAM_DATAM	   (0xffff<<0)	/* mask for above */
#define   ES_1371_SRC_RAM_DATAI(i) (((i)>>0)&0xffff)/* current value of the sample rate converter */

#define ES_REG_1371_LEGACY 0x18	/* W/R: Legacy control/status register */
#define   ES_1371_JFAST		(1<<31)		/* fast joystick timing */
#define   ES_1371_HIB		(1<<30)		/* host interrupt blocking enable */
#define   ES_1371_VSB		(1<<29)		/* SB; 0 = addr 0x220xH, 1 = 0x22FxH */
#define   ES_1371_VMPUO(o)	(((o)&0x03)<<27)/* base register address; 0 = 0x320xH; 1 = 0x330xH; 2 = 0x340xH; 3 = 0x350xH */
#define   ES_1371_VMPUM		(0x03<<27)	/* mask for above */
#define   ES_1371_VMPUI(i)	(((i)>>27)&0x03)/* base register address */
#define   ES_1371_VCDCO(o)	(((o)&0x03)<<25)/* CODEC; 0 = 0x530xH; 1 = undefined; 2 = 0xe80xH; 3 = 0xF40xH */
#define   ES_1371_VCDCM		(0x03<<25)	/* mask for above */
#define   ES_1371_VCDCI(i)	(((i)>>25)&0x03)/* CODEC address */
#define   ES_1371_FIRQ		(1<<24)		/* force an interrupt */
#define   ES_1371_SDMACAP	(1<<23)		/* enable event capture for slave DMA controller */
#define   ES_1371_SPICAP	(1<<22)		/* enable event capture for slave IRQ controller */
#define   ES_1371_MDMACAP	(1<<21)		/* enable event capture for master DMA controller */
#define   ES_1371_MPICAP	(1<<20)		/* enable event capture for master IRQ controller */
#define   ES_1371_ADCAP		(1<<19)		/* enable event capture for ADLIB register; 0x388xH */
#define   ES_1371_SVCAP		(1<<18)		/* enable event capture for SB registers */
#define   ES_1371_CDCCAP	(1<<17)		/* enable event capture for CODEC registers */
#define   ES_1371_BACAP		(1<<16)		/* enable event capture for SoundScape base address */
#define   ES_1371_EXI(i)	(((i)>>8)&0x07)	/* event number */
#define   ES_1371_AI(i)		(((i)>>3)&0x1f)	/* event significant I/O address */
#define   ES_1371_WR		(1<<2)	/* event capture; 0 = read; 1 = write */
#define   ES_1371_LEGINT	(1<<0)	/* interrupt for legacy events; 0 = interrupt did occur */

#define ES_REG_CHANNEL_STATUS 0x1c /* R/W: first 32-bits from S/PDIF channel status block, es1373 */

#define ES_REG_SERIAL	0x20	/* R/W: Serial interface control register */
#define   ES_1371_DAC_TEST	(1<<22)		/* DAC test mode enable */
#define   ES_P2_END_INCO(o)	(((o)&0x07)<<19)/* binary offset value to increment / loop end */
#define   ES_P2_END_INCM	(0x07<<19)	/* mask for above */
#define   ES_P2_END_INCI(i)	(((i)>>16)&0x07)/* binary offset value to increment / loop end */
#define   ES_P2_ST_INCO(o)	(((o)&0x07)<<16)/* binary offset value to increment / start */
#define   ES_P2_ST_INCM		(0x07<<16)	/* mask for above */
#define   ES_P2_ST_INCI(i)	(((i)<<16)&0x07)/* binary offset value to increment / start */
#define   ES_R1_LOOP_SEL	(1<<15)		/* ADC; 0 - loop mode; 1 = stop mode */
#define   ES_P2_LOOP_SEL	(1<<14)		/* DAC2; 0 - loop mode; 1 = stop mode */
#define   ES_P1_LOOP_SEL	(1<<13)		/* DAC1; 0 - loop mode; 1 = stop mode */
#define   ES_P2_PAUSE		(1<<12)		/* DAC2; 0 - play mode; 1 = pause mode */
#define   ES_P1_PAUSE		(1<<11)		/* DAC1; 0 - play mode; 1 = pause mode */
#define   ES_R1_INT_EN		(1<<10)		/* ADC interrupt enable */
#define   ES_P2_INT_EN		(1<<9)		/* DAC2 interrupt enable */
#define   ES_P1_INT_EN		(1<<8)		/* DAC1 interrupt enable */
#define   ES_P1_SCT_RLD		(1<<7)		/* force sample counter reload for DAC1 */
#define   ES_P2_DAC_SEN		(1<<6)		/* when stop mode: 0 - DAC2 play back zeros; 1 = DAC2 play back last sample */
#define   ES_R1_MODEO(o)	(((o)&0x03)<<4)	/* ADC mode; 0 = 8-bit mono; 1 = 8-bit stereo; 2 = 16-bit mono; 3 = 16-bit stereo */
#define   ES_R1_MODEM		(0x03<<4)	/* mask for above */
#define   ES_R1_MODEI(i)	(((i)>>4)&0x03)
#define   ES_P2_MODEO(o)	(((o)&0x03)<<2)	/* DAC2 mode; -- '' -- */
#define   ES_P2_MODEM		(0x03<<2)	/* mask for above */
#define   ES_P2_MODEI(i)	(((i)>>2)&0x03)
#define   ES_P1_MODEO(o)	(((o)&0x03)<<0)	/* DAC1 mode; -- '' -- */
#define   ES_P1_MODEM		(0x03<<0)	/* mask for above */
#define   ES_P1_MODEI(i)	(((i)>>0)&0x03)

#define ES_REG_DAC1_COUNT 0x24	/* R/W: DAC1 sample count register */
#define ES_REG_DAC2_COUNT 0x28	/* R/W: DAC2 sample count register */
#define ES_REG_ADC_COUNT  0x2c	/* R/W: ADC sample count register */
#define   ES_REG_CURR_COUNT(i)  (((i)>>16)&0xffff)
#define   ES_REG_COUNTO(o)	(((o)&0xffff)<<0)
#define   ES_REG_COUNTM		(0xffff<<0)
#define   ES_REG_COUNTI(i)	(((i)>>0)&0xffff)

#define ES_REG_DAC1_FRAME 0x30	/* R/W: PAGE 0x0c; DAC1 frame address */
#define ES_REG_DAC1_SIZE  0x34	/* R/W: PAGE 0x0c; DAC1 frame size */
#define ES_REG_DAC2_FRAME 0x38	/* R/W: PAGE 0x0c; DAC2 frame address */
#define ES_REG_DAC2_SIZE  0x3c	/* R/W: PAGE 0x0c; DAC2 frame size */
#define ES_REG_ADC_FRAME  0x30	/* R/W: PAGE 0x0d; ADC frame address */
#define ES_REG_ADC_SIZE	  0x34	/* R/W: PAGE 0x0d; ADC frame size */
#define   ES_REG_FCURR_COUNTO(o) (((o)&0xffff)<<16)
#define   ES_REG_FCURR_COUNTM    (0xffff<<16)
#define   ES_REG_FCURR_COUNTI(i) (((i)>>14)&0x3fffc)
#define   ES_REG_FSIZEO(o)	 (((o)&0xffff)<<0)
#define   ES_REG_FSIZEM		 (0xffff<<0)
#define   ES_REG_FSIZEI(i)	 (((i)>>0)&0xffff)
#define ES_REG_PHANTOM_FRAME 0x38 /* R/W: PAGE 0x0d: phantom frame address */
#define ES_REG_PHANTOM_COUNT 0x3c /* R/W: PAGE 0x0d: phantom frame count */

#define ES_REG_UART_FIFO  0x30	/* R/W: PAGE 0x0e; UART FIFO register */
#define   ES_REG_UF_VALID	 (1<<8)
#define   ES_REG_UF_BYTEO(o)	 (((o)&0xff)<<0)
#define   ES_REG_UF_BYTEM	 (0xff<<0)
#define   ES_REG_UF_BYTEI(i)	 (((i)>>0)&0xff)